#include "WorkerDesktop.hpp"
#include "MessageHandler.hpp"
#include "ServiceDesktop.hpp"

using namespace cpp_freertos;

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr)
    : sys::Worker(ownerServicePtr), Timer("WorkerDesktop::Timeout", cpp_freertos::Ticks::MsToTicks(sdesktop::file_transfer_timeout), false),
      ownerService(ownerServicePtr), parser(ownerServicePtr), fileDes(nullptr)
{
    LOG_DEBUG("ctor");
}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);
    cdcVcomStruct = bsp::usbInit();
    if (cdcVcomStruct == NULL) {
        LOG_ERROR("won't start desktop service without serial port");
        return false;
    }

    receiveQueue                         = Worker::getQueueByName(WorkerDesktop::RECEIVE_QUEUE_BUFFER_NAME);
    parserFSM::MessageHandler::sendQueue = Worker::getQueueByName(WorkerDesktop::SEND_QUEUE_BUFFER_NAME);

    if (xTaskCreate(
        &WorkerDesktop::deviceTask, "WorkerDesktop::deviceTask", 2048L / sizeof(portSTACK_TYPE), this, 2, NULL) !=
        pdPASS) {
        LOG_ERROR("xTaskCreate failed");
    }

    return true;
}

bool WorkerDesktop::deinit(void)
{
    Worker::deinit();

    if (fileDes != nullptr)
        vfs.fclose(fileDes);

    return true;
}

bool WorkerDesktop::handleMessage(uint32_t queueID)
{
    QueueHandle_t queue = queues[queueID];

    std::string qname = queueNameMap[queue];
    LOG_INFO("received data from queue: %s", qname.c_str());
    static std::string *sendMsg;
    static std::string receiveMsg;

    if (qname == RECEIVE_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &receiveMsg, 0) != pdTRUE) {
            LOG_ERROR("handleMessage failed to receive from \"%s\"", RECEIVE_QUEUE_BUFFER_NAME.c_str());
            return false;
        }
        else {
            parser.processMessage(receiveMsg);
        }
    }

    // TODO: Consider moving sendBuffer receive to bsp driver
    if (qname == SEND_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &sendMsg, 0) != pdTRUE)
            return false;

        VirtualComSend(cdcVcomStruct, sendMsg->c_str(), sendMsg->length());
    }

    return true;
}

void WorkerDesktop::deviceTask(void *handle)
{
    WorkerDesktop *worker = static_cast<WorkerDesktop *>(handle);
    std::unique_ptr<char[]> input(new char[SERIAL_BUFFER_LEN]);
    usb_cdc_vcom_struct_t *cdcVcom = worker->getCdcVcomStruct();
    xQueueHandle receiveQueue      = worker->getReceiveQueue();
    size_t len;

    if (!input) {
        LOG_ERROR("Buffer allocation failed");
        return;
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    LOG_INFO("Task started");

    while (1) {
        len = VirtualComRecv(cdcVcom, input.get(), SERIAL_BUFFER_LEN);
        if (len > 0) {
            if (worker->getTransferType() != WorkerDesktop::TransferType::JSONCommands) {
                // file tranafer in progress, write/read data from the stream, ignore queues
                worker->transferDataReceived(input.get(), len);
            }
            else if (uxQueueSpacesAvailable(receiveQueue) != 0) {
                static std::string receiveMessage;
                receiveMessage = std::string(input.get(), len);

                xQueueSend(receiveQueue, &receiveMessage, portMAX_DELAY);
            }
        }
        else {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
}

WorkerDesktop::TransferType WorkerDesktop::getTransferType()
{
    return currentTransferType;
}

sys::ReturnCodes WorkerDesktop::startDownload(const fs::path &filePath, const uint32_t fileSize)
{
    fileDes = vfs.fopen(filePath.c_str(), "w");

    if (fileDes == nullptr)
        return sys::ReturnCodes::Failure;

    if (fileSize <= 0)
        return sys::ReturnCodes::Failure;

    if (Start(cpp_freertos::Ticks::MsToTicks(1000)) == false)
        return sys::ReturnCodes::Failure;

    writeFileSizeExpected = fileSize;
    currentTransferType = TransferType::DownloadFile;
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes WorkerDesktop::startUpload(const fs::path &filePath, const uint32_t fileSize)
{
    currentTransferType = TransferType::UploadFile;
    return sys::ReturnCodes::Success;
}

void WorkerDesktop::stopTransfer()
{
    LOG_DEBUG("stopTransfer");
    // close the file descriptor
    vfs.fclose(fileDes);

    // stop the timeout timer
    Stop();

    // reset all counters
    writeFileSizeExpected = 0;
    writeFileDataWritten  = 0;
    parser.setState(parserFSM::State::NoMsg);
    currentTransferType = TransferType::JSONCommands;
}

void WorkerDesktop::transferDataReceived(const char *data, uint32_t dataLen)
{
    if (getTransferType() == TransferType::DownloadFile) {
        Reset(cpp_freertos::Ticks::MsToTicks(sdesktop::file_transfer_timeout));

        if (data == nullptr || dataLen == 0) {
            LOG_ERROR("transferDataReceived invalid data");
            return;
        }

        const uint32_t bytesWritten = vfs.fwrite(data, 1, dataLen, fileDes);

        if (bytesWritten != dataLen) {
            LOG_ERROR("transferDataReceived vfs write failed bytesWritten=%lu != dataLen=%lu", bytesWritten, dataLen);
            return;
        }

        writeFileDataWritten += dataLen;

        if (writeFileDataWritten >= writeFileSizeExpected) {
            stopTransfer();
        }
    }
}

void WorkerDesktop::Run()
{
    LOG_DEBUG("timeout timer: run");

    if (getTransferType() != TransferType::JSONCommands) {
        LOG_DEBUG("timeout timer: stopping transfer");
        stopTransfer();
    }
}