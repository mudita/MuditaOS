// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include "parser/MessageHandler.hpp"
#include "parser/ParserFSM.hpp"
#include "endpoints/Context.hpp"
#include <bsp/usb/usb.hpp>
#include <log/log.hpp>
#include <projdefs.h>
#include <queue.h>

#include <map>
#include <vector>
inline constexpr auto uploadFailedMessage = "file upload terminated before all data transferred";

static void uploadFileFailedResponse()
{
    LOG_ERROR("Upload file failed, timeout");
    parserFSM::Context response;
    response.setResponseStatus(parserFSM::http::Code::InternalServerError);
    response.setEndpoint(parserFSM::EndpointType::filesystemUpload);

    json11::Json responseJson = json11::Json::object{
        {parserFSM::json::status, uploadFailedMessage},
    };
    response.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(response.createSimpleResponse());
}

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr)
    : sys::Worker(ownerServicePtr),
      Timer("WorkerDesktop::Timeout", cpp_freertos::Ticks::MsToTicks(sdesktop::file_transfer_timeout), false),
      ownerService(ownerServicePtr), parser(ownerServicePtr), fileDes(nullptr)
{}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);

    receiveQueue                         = Worker::getQueueByName(sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
    parserFSM::MessageHandler::sendQueue = Worker::getQueueByName(sdesktop::SEND_QUEUE_BUFFER_NAME);

    return static_cast<bool>(bsp::usbInit(receiveQueue, this));
}

bool WorkerDesktop::deinit(void)
{
    LOG_DEBUG("deinit");

    if (fileDes != nullptr) {
        LOG_DEBUG("deinit close opened fileDes");
        vfs.fclose(fileDes);
    }

    Worker::deinit();

    LOG_DEBUG("deinit end");
    return true;
}

bool WorkerDesktop::handleMessage(uint32_t queueID)
{
    QueueHandle_t queue = queues[queueID];

    std::string qname = queueNameMap[queue];
    LOG_INFO("handleMessage received data from queue: %s", qname.c_str());
    static std::string *sendMsg;
    static std::string receiveMsg;

    if (qname == sdesktop::RECEIVE_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &receiveMsg, 0) != pdTRUE) {
            LOG_ERROR("handleMessage failed to receive from \"%s\"", sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
            return false;
        }
        else {
            parser.processMessage(receiveMsg);
        }
    }
    else if (qname == sdesktop::SEND_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &sendMsg, 0) != pdTRUE) {
            LOG_ERROR("handleMessage xQueueReceive failed for %s size %d bytes",
                      sdesktop::SEND_QUEUE_BUFFER_NAME,
                      static_cast<unsigned int>(sendMsg->length()));
            return false;
        }
        else {
            LOG_DEBUG("handeMessage sending %d bytes using usbCDCSend", static_cast<unsigned int>(sendMsg->length()));
        }

        bsp::usbCDCSend(sendMsg);
    }
    else {
        LOG_INFO("handeMessage got message on an unhandled queue");
    }

    return true;
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
    rawModeEnabled        = true;

    LOG_DEBUG("startDownload all checks passed starting download");
    return sys::ReturnCodes::Success;
}

void WorkerDesktop::stopTransfer(const bool removeDestinationFile)
{
    LOG_DEBUG("stopTransfer %s", removeDestinationFile ? "remove desination file" : "");
    parser.setState(parserFSM::State::NoMsg);
    rawModeEnabled = false;

    parserFSM::Context response;
    response.setResponseStatus(removeDestinationFile ? parserFSM::http::Code::NotAcceptable
                                                     : parserFSM::http::Code::Accepted);
    response.setEndpoint(parserFSM::EndpointType::filesystemUpload);
    json11::Json responseJson = json11::Json::object{{parserFSM::json::fileSize, std::to_string(writeFileDataWritten)},
                                                     {parserFSM::json::fileName, filePath.filename().c_str()}};
    response.setResponseBody(responseJson);

    // close the file descriptor
    vfs.fclose(fileDes);

    // stop the timeout timer
    Stop();

    // reset all counters
    writeFileSizeExpected = 0;
    writeFileDataWritten  = 0;

    if (removeDestinationFile) {
        if (vfs.remove(filePath.c_str()) != 0) {
            LOG_ERROR("stopTransfer can't delete file(requested) %s", filePath.c_str());
        }
    }

    parserFSM::MessageHandler::putToSendQueue(response.createSimpleResponse());
}

void WorkerDesktop::rawDataReceived(void *dataPtr, uint32_t dataLen)
{
    if (getRawMode()) {
        Reset(cpp_freertos::Ticks::MsToTicks(sdesktop::file_transfer_timeout));

        if (dataPtr == nullptr || dataLen == 0) {
            LOG_ERROR("transferDataReceived invalid data");
            return;
        }

        const uint32_t bytesWritten = vfs.fwrite(dataPtr, 1, dataLen, fileDes);

        if (bytesWritten != dataLen) {
            LOG_ERROR("transferDataReceived vfs write failed bytesWritten=%" PRIu32 " != dataLen=%" PRIu32,
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

    if (getRawMode()) {
        LOG_DEBUG("timeout timer: stopping transfer");
        uploadFileFailedResponse();
        stopTransfer(true);
    }
}

bool WorkerDesktop::getRawMode()
{
    return rawModeEnabled;
}