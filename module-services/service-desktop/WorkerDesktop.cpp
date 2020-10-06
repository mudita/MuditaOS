#include "WorkerDesktop.hpp"
#include "MessageHandler.hpp"
#include "ServiceDesktop.hpp"

#ifdef TARGET_Linux
int VirtualComSend(usb_cdc_vcom_struct_t *cdcVcom, const void *data, size_t length)
{
    return -1;
}
int VirtualComRecv(usb_cdc_vcom_struct_t *cdcVcom, void *data, size_t length)
{
    return -1;
}
namespace bsp
{
    usb_cdc_vcom_struct_t *usbInit()
    {
        return nullptr;
    }
} // namespace bsp
#endif

using namespace cpp_freertos;

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr)
    : sys::Worker(ownerServicePtr),
      Timer("WorkerDesktop::Timeout", cpp_freertos::Ticks::MsToTicks(sdesktop::file_transfer_timeout), false),
      ownerService(ownerServicePtr), parser(ownerServicePtr), fileDes(nullptr)
{}

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
        LOG_ERROR("deviceTask buffer allocation failed");
        return;
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);

    LOG_INFO("deviceTask task started");

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

sys::ReturnCodes WorkerDesktop::startDownload(const fs::path &destinationPath, const uint32_t fileSize)
{
    filePath = destinationPath;
    fileDes  = vfs.fopen(filePath.c_str(), "w");

    if (fileDes == nullptr)
        return sys::ReturnCodes::Failure;

    if (fileSize <= 0)
        return sys::ReturnCodes::Failure;

    if (Start(cpp_freertos::Ticks::MsToTicks(1000)) == false)
        return sys::ReturnCodes::Failure;

    writeFileSizeExpected = fileSize;
    currentTransferType   = TransferType::DownloadFile;

    LOG_DEBUG("startDownload all checks passed starting download");
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes WorkerDesktop::startUpload(const fs::path &sourcePath, const uint32_t fileSize)
{
    currentTransferType = TransferType::UploadFile;
    return sys::ReturnCodes::Success;
}

void WorkerDesktop::stopTransfer(const bool removeDestinationFile)
{
    LOG_DEBUG("stopTransfer %s", removeDestinationFile ? "remove desination file" : "");
    parser.setState(parserFSM::State::NoMsg);
    currentTransferType = TransferType::JSONCommands;

    // close the file descriptor
    vfs.fclose(fileDes);

    // stop the timeout timer
    Stop();

    // reset all counters
    writeFileSizeExpected = 0;
    writeFileDataWritten  = 0;

    if (removeDestinationFile) {
        if (vfs.remove(filePath.c_str()) != 0) {
            int currentErrno = vfs.getErrno();
            LOG_ERROR("stopTransfer can't delete file(requested) %s errno: %d/%s",
                      filePath.c_str(),
                      currentErrno,
                      strerror(currentErrno));
        }
    }
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
            LOG_ERROR("transferDataReceived vfs write failed bytesWritten=%" PRIu32 " != dataLen=%" PRIu32 "",
                      bytesWritten,
                      dataLen);
            return;
        }

        writeFileDataWritten += dataLen;

        if (writeFileDataWritten >= writeFileSizeExpected) {
            LOG_INFO("transferDataReceived all data transferred, stop now");
            stopTransfer(false);
        }
    }
    else {
        LOG_DEBUG("transferDataReceived not in a transfer state");
    }
}

void WorkerDesktop::Run()
{
    LOG_DEBUG("timeout timer: run");

    if (getTransferType() != TransferType::JSONCommands) {
        LOG_DEBUG("timeout timer: stopping transfer");
        stopTransfer(true);
    }
}