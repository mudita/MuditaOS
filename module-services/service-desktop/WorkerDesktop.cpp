// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include "service-desktop/endpoints/EndpointFactory.hpp"
#include "service-desktop/DesktopMessages.hpp"
#include "parser/MessageHandler.hpp"
#include "parser/ParserFSM.hpp"
#include "endpoints/Context.hpp"
#include "Timers/TimerFactory.hpp"

#include <bsp/usb/usb.hpp>
#include <log.hpp>
#include <crc32.h>

#include <map>
#include <vector>
#include <filesystem>
#include <module-services/service-desktop/service-desktop/DesktopMessages.hpp>
#include "SystemManager/messages/SentinelRegistrationMessage.hpp"

inline constexpr auto uploadFailedMessage = "file upload terminated before all data transferred";

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr,
                             const sdesktop::USBSecurityModel &securityModel,
                             const std::string serialNumber)
    : sys::Worker(ownerServicePtr, sdesktop::worker_stack), fileDes(nullptr), securityModel(securityModel),
      serialNumber(serialNumber), ownerService(ownerServicePtr), parser(ownerServicePtr)
{}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    LOG_ERROR("sadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsad");
    delete x;
    delete x;
    LOG_ERROR("sadsadsadsadsad %d", *x);
    x = new int(200000);
    delete x;
    delete x;
    LOG_ERROR("sadsadsadsadsad %d", *x);

    Worker::init(queues);

    irqQueue     = Worker::getQueueHandleByName(sdesktop::IRQ_QUEUE_BUFFER_NAME);
    receiveQueue = Worker::getQueueHandleByName(sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
    parserFSM::MessageHandler::setSendQueueHandle(Worker::getQueueHandleByName(sdesktop::SEND_QUEUE_BUFFER_NAME));

    cpuSentinel                  = std::make_shared<sys::CpuSentinel>("WorkerDesktop", ownerService);
    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    ownerService->bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    usbSuspendTimer = sys::TimerFactory::createSingleShotTimer(
        ownerService, "usbSuspend", constants::usbSuspendTimeout, [this](sys::Timer &) { suspendUsb(); });

    bsp::usbInitParams initParams = {receiveQueue, irqQueue, this, serialNumber.c_str()};

    return (bsp::usbInit(initParams) < 0) ? false : true;
}

bool WorkerDesktop::deinit(void)
{
    unsigned int maxcount = 10;
    while (parser.getCurrentState() != parserFSM::State::NoMsg && --maxcount > 0) {
        vTaskDelay(300);
    }
    LOG_INFO("we can deinit worker now");

    /// additional wait to flush on serial - we should wait for data sent...
    vTaskDelay(3000);

    if (fileDes != nullptr) {
        LOG_DEBUG("deinit close opened fileDes");
        fclose(fileDes);
    }

    bsp::usbDeinit();

    Worker::deinit();

    LOG_DEBUG("deinit end");
    return true;
}

bool WorkerDesktop::reinit(const std::filesystem::path &path)
{
    LOG_DEBUG("Reinit USB begin");

    bsp::usbReinit(path.c_str());

    LOG_DEBUG("Reinit USB end");
    return true;
}

bool WorkerDesktop::handleMessage(uint32_t queueID)
{
    auto &queue       = queues[queueID];
    const auto &qname = queue->GetQueueName();

    if (qname == sdesktop::RECEIVE_QUEUE_BUFFER_NAME) {
        std::string *receivedMsg = nullptr;
        if (!queue->Dequeue(&receivedMsg, 0)) {
            LOG_ERROR("handleMessage failed to receive from \"%s\"", sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
            return false;
        }

        auto factory = std::make_unique<SecuredEndpointFactory>(securityModel.getEndpointSecurity());
        auto handler = std::make_unique<parserFSM::MessageHandler>(ownerService, std::move(factory));

        parser.setMessageHandler(std::move(handler));
        parser.processMessage(std::move(*receivedMsg));

        delete receivedMsg;
    }
    else if (qname == sdesktop::SEND_QUEUE_BUFFER_NAME) {
        std::string *sendMsg = nullptr;
        if (!queue->Dequeue(&sendMsg, 0)) {
            LOG_ERROR("handleMessage xQueueReceive failed for %s.", sdesktop::SEND_QUEUE_BUFFER_NAME);
            return false;
        }

        bsp::usbCDCSend(sendMsg);
        delete sendMsg;
    }
    else if (qname == SERVICE_QUEUE_NAME) {
        auto &serviceQueue = getServiceQueue();
        sys::WorkerCommand cmd;

        if (serviceQueue.Dequeue(&cmd, 0)) {
            switch (static_cast<Command>(cmd.command)) {
            case Command::CancelTransfer:
                transferTimeoutHandler();
                break;
            }
        }
        else {
            LOG_ERROR("handleMessage xQueueReceive failed for %s.", SERVICE_QUEUE_NAME.c_str());
            return false;
        }
    }
    else if (qname == sdesktop::IRQ_QUEUE_BUFFER_NAME) {
        bsp::USBDeviceStatus notification = bsp::USBDeviceStatus::Disconnected;
        if (!queue->Dequeue(&notification, 0)) {
            LOG_ERROR("handleMessage xQueueReceive failed for %s.", sdesktop::IRQ_QUEUE_BUFFER_NAME);
            return false;
        }

        LOG_DEBUG("USB status: %d", static_cast<int>(notification));

        if (notification == bsp::USBDeviceStatus::Connected) {
            usbSuspendTimer.stop();
            ownerService->bus.sendUnicast(std::make_shared<sdesktop::usb::USBConnected>(),
                                          service::name::service_desktop);
        }
        else if (notification == bsp::USBDeviceStatus::Configured) {
            cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyHz::Level_4);
            if (usbStatus == bsp::USBDeviceStatus::Connected) {
                ownerService->bus.sendUnicast(std::make_shared<sdesktop::usb::USBConfigured>(
                                                  sdesktop::usb::USBConfigurationType::firstConfiguration),
                                              service::name::service_desktop);
            }
            else {
                ownerService->bus.sendUnicast(std::make_shared<sdesktop::usb::USBConfigured>(
                                                  sdesktop::usb::USBConfigurationType::reconfiguration),
                                              service::name::service_desktop);
            }
        }
        else if (notification == bsp::USBDeviceStatus::Disconnected) {
            usbSuspendTimer.start();
            cpuSentinel->ReleaseMinimumFrequency();
            ownerService->bus.sendUnicast(std::make_shared<sdesktop::usb::USBDisconnected>(),
                                          service::name::service_desktop);
        }
        usbStatus = notification;
    }
    else {
        LOG_INFO("handeMessage got message on an unhandled queue");
    }

    return true;
}

sys::ReturnCodes WorkerDesktop::startDownload(const std::filesystem::path &destinationPath,
                                              uint32_t fileSize,
                                              std::string fileCrc32)
{
    filePath = destinationPath;
    fileDes  = fopen(filePath.c_str(), "w");

    if (fileDes == nullptr)
        return sys::ReturnCodes::Failure;

    if (fileSize <= 0)
        return sys::ReturnCodes::Failure;

    startTransferTimer();

    writeFileSizeExpected = fileSize;
    expectedFileCrc32     = fileCrc32;
    rawModeEnabled        = true;

    digestCrc32.reset();

    LOG_DEBUG("startDownload all checks passed starting download");
    return sys::ReturnCodes::Success;
}

void WorkerDesktop::startTransferTimer()
{
    auto msg = std::make_shared<sdesktop::transfer::TransferTimerState>(
        sdesktop::transfer::TransferTimerState::Request::Start);
    ownerService->bus.sendUnicast(std::move(msg), service::name::service_desktop);
}

void WorkerDesktop::stopTransferTimer()
{
    auto msg =
        std::make_shared<sdesktop::transfer::TransferTimerState>(sdesktop::transfer::TransferTimerState::Request::Stop);
    ownerService->bus.sendUnicast(std::move(msg), service::name::service_desktop);
}
void WorkerDesktop::reloadTransferTimer()
{
    auto msg = std::make_shared<sdesktop::transfer::TransferTimerState>(
        sdesktop::transfer::TransferTimerState::Request::Reload);
    ownerService->bus.sendUnicast(std::move(msg), service::name::service_desktop);
}

void WorkerDesktop::stopTransfer(const TransferFailAction action)
{
    parser.setState(parserFSM::State::NoMsg);
    rawModeEnabled = false;

    auto removeFile     = (action == TransferFailAction::removeDesitnationFile);
    auto responseStatus = removeFile ? parserFSM::http::Code::NotAcceptable : parserFSM::http::Code::Accepted;

    const auto fileCrc32  = digestCrc32.getHash();
    const auto crc32Match = expectedFileCrc32.empty() ? true : (expectedFileCrc32.compare(fileCrc32) == 0);

    if (!crc32Match && !removeFile) {
        responseStatus = parserFSM::http::Code::NotAcceptable;
        removeFile     = true;

        LOG_ERROR(
            "File transfer CRC32 mismatch, expected: %s, actual: %s", expectedFileCrc32.c_str(), fileCrc32.c_str());
    }

    parserFSM::Context responseContext;
    responseContext.setResponseStatus(responseStatus);
    responseContext.setEndpoint(parserFSM::EndpointType::filesystemUpload);
    json11::Json responseJson = json11::Json::object{{parserFSM::json::fileSize, std::to_string(writeFileDataWritten)},
                                                     {parserFSM::json::fileName, filePath.filename().c_str()},
                                                     {parserFSM::json::fileCrc32, fileCrc32.c_str()}};
    responseContext.setResponseBody(responseJson);

    // close the file descriptor
    std::fclose(fileDes);

    // stop the timeout timer
    stopTransferTimer();

    // reset all counters
    writeFileSizeExpected = 0;
    writeFileDataWritten  = 0;

    if (removeFile) {
        try {
            if (!std::filesystem::remove(filePath)) {
                LOG_ERROR("can't delete file");
            }
            LOG_DEBUG("Deleted file");
        }
        catch (const std::filesystem::filesystem_error &fsError) {
            LOG_ERROR("Removing file failed");
        }
    }

    parserFSM::MessageHandler::putToSendQueue(responseContext.createSimpleResponse());
}

void WorkerDesktop::rawDataReceived(void *dataPtr, uint32_t dataLen)
{
    if (getRawMode()) {
        reloadTransferTimer();

        if (dataPtr == nullptr || dataLen == 0) {
            LOG_ERROR("transferDataReceived invalid data");
            return;
        }

        digestCrc32.add(dataPtr, dataLen);

        const uint32_t bytesWritten = std::fwrite(dataPtr, 1, dataLen, fileDes);

        if (bytesWritten != dataLen) {
            LOG_ERROR("transferDataReceived write failed bytesWritten=%" PRIu32 " != dataLen=%" PRIu32,
                      bytesWritten,
                      dataLen);
            return;
        }

        writeFileDataWritten += dataLen;

        if (writeFileDataWritten >= writeFileSizeExpected) {
            LOG_INFO("transferDataReceived all data transferred, stop now");
            stopTransfer(TransferFailAction::doNothing);
        }
    }
    else {
        LOG_DEBUG("transferDataReceived not in a transfer state");
    }
}

void WorkerDesktop::cancelTransferOnTimeout()
{
    LOG_DEBUG("timeout timer: run");
    sendCommand({.command = static_cast<uint32_t>(Command::CancelTransfer), .data = nullptr});
}

void WorkerDesktop::transferTimeoutHandler()
{
    if (getRawMode()) {
        LOG_DEBUG("timeout timer: stopping transfer");
        uploadFileFailedResponse();
        stopTransfer(TransferFailAction::removeDesitnationFile);
    }
}

bool WorkerDesktop::getRawMode() const noexcept
{
    return rawModeEnabled;
}

void WorkerDesktop::uploadFileFailedResponse()
{
    LOG_ERROR("Upload file failed, timeout");
    parserFSM::Context responseContext;
    responseContext.setResponseStatus(parserFSM::http::Code::InternalServerError);
    responseContext.setEndpoint(parserFSM::EndpointType::filesystemUpload);

    json11::Json responseJson = json11::Json::object{
        {parserFSM::json::status, uploadFailedMessage},
    };
    responseContext.setResponseBody(responseJson);
    parserFSM::MessageHandler::putToSendQueue(responseContext.createSimpleResponse());
}

void WorkerDesktop::suspendUsb()
{
    bsp::usbSuspend();
}
