// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include "service-desktop/DeviceColour.hpp"
#include <endpoints/EndpointFactory.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <endpoints/message/Sender.hpp>
#include <MessageHandler.hpp>

#include <bsp/usb/usb.hpp>
#include <log/log.hpp>
#include <crc32.h>

#include <utility>
#include <vector>
#include "system/messages/SentinelRegistrationMessage.hpp"

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr,
                             std::function<void()> messageProcessedCallback,
                             const sdesktop::USBSecurityModel &securityModel,
                             const std::string &serialNumber,
                             const std::string &caseColour,
                             const std::string &rootPath)
    : sys::Worker(ownerServicePtr, sdesktop::workerStackSize), securityModel(securityModel), serialNumber(serialNumber),
      caseColour(caseColour), rootPath(rootPath), ownerService(ownerServicePtr), parser(ownerServicePtr),
      messageProcessedCallback(std::move(messageProcessedCallback))
{}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    if (initialized) {
        return true;
    }

    Worker::init(queues);

    irqQueue     = Worker::getQueueHandleByName(sdesktop::irqQueueName);
    receiveQueue = Worker::getQueueHandleByName(sdesktop::cdcReceiveQueueName);
    sdesktop::endpoints::sender::setSendQueueHandle(Worker::getQueueHandleByName(sdesktop::cdcSendQueueName));

    cpuSentinel                  = std::make_shared<sys::CpuSentinel>("WorkerDesktop", ownerService);
    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    ownerService->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

    const bsp::usbInitParams initParams = {receiveQueue,
                                           irqQueue,
                                           serialNumber,
                                           device_colour::getColourVersion(caseColour),
                                           rootPath,
                                           securityModel.isSecurityEnabled()};

    initialized = bsp::usbInit(initParams) == 0;

    cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_4);

    return initialized;
}

void WorkerDesktop::closeWorker(void)
{
    if (!initialized) {
        return;
    }

    unsigned int maxcount = 10;
    while (parser.getCurrentState() != sdesktop::endpoints::State::NoMsg && --maxcount > 0) {
        vTaskDelay(300);
    }

    initialized = false;
    /// additional wait to flush on serial - we should wait for data sent...
    vTaskDelay(500);

    bsp::usbDeinit();

    close();

    cpuSentinel->ReleaseMinimumFrequency();
    auto sentinelRemoveMsg = std::make_shared<sys::SentinelRemovalMessage>("WorkerDesktop");
    auto result            = ownerService->bus.sendUnicastSync(sentinelRemoveMsg, service::name::system_manager, 1000);
    if (result.first != sys::ReturnCodes::Success) {
        LOG_ERROR("Sentinel %s could not be removed!", cpuSentinel->GetName().c_str());
    }
    cpuSentinel.reset();
}

void WorkerDesktop::reset()
{
    initialized = false;
    configured  = false;

    bsp::usbDeinit();

    const bsp::usbInitParams initParams = {receiveQueue,
                                           irqQueue,
                                           serialNumber,
                                           device_colour::getColourVersion(caseColour),
                                           rootPath,
                                           securityModel.isSecurityEnabled()};

    initialized = bsp::usbInit(initParams) == 0;

    if (initialized) {
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConnected>(),
                                        sys::BusChannel::USBNotifications);
    }
}

void WorkerDesktop::notify(Signal command)
{
    if (auto queue = getQueueByName(sdesktop::signallingQueueName); !queue->Overwrite(&command)) {
        LOG_ERROR("Unable to overwrite the command in the commands queue '%s'.", sdesktop::signallingQueueName);
    }
}

bool WorkerDesktop::handleMessage(std::uint32_t queueID)
{
    bool result       = false;
    auto &queue       = queues[queueID];
    const auto &qname = queue->GetQueueName();

    if (qname == sdesktop::cdcReceiveQueueName) {
        result = handleReceiveQueueMessage(queue);
    }
    else if (qname == sdesktop::cdcSendQueueName) {
        result = handleSendQueueMessage(queue);
    }
    else if (qname == SERVICE_QUEUE_NAME) {
        result = handleServiceQueueMessage(queue);
    }
    else if (qname == sdesktop::irqQueueName) {
        result = handleIrqQueueMessage(queue);
    }
    else if (qname == sdesktop::signallingQueueName) {
        result = handleSignallingQueueMessage(queue);
    }
    else {
        LOG_WARN("Unhandled queue '%s'!", qname.c_str());
    }

    return result;
}

bool WorkerDesktop::handleReceiveQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    if (!initialized) {
        return false;
    }
    std::string *receivedMsg = nullptr;
    if (!queue->Dequeue(&receivedMsg, 0)) {
        LOG_ERROR("Failed dequeue for '%s'.", sdesktop::cdcReceiveQueueName);
        return false;
    }

    using namespace sdesktop::endpoints;
    auto factory = EndpointFactory::create(securityModel.getEndpointSecurity().access);
    auto handler = std::make_unique<MessageHandler>(ownerService, messageProcessedCallback, std::move(factory));

    parser.setMessageHandler(std::move(handler));
    parser.processMessage(std::move(*receivedMsg));

    delete receivedMsg;
    return true;
}

bool WorkerDesktop::handleSendQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    if (!initialized) {
        return false;
    }
    std::string *sendMsg = nullptr;
    if (!queue->Dequeue(&sendMsg, 0)) {
        LOG_ERROR("Failed dequeue for '%s'.", sdesktop::cdcSendQueueName);
        return false;
    }

    const std::uint32_t dataSent = bsp::usbCDCSend(sendMsg);
    if (dataSent != sendMsg->length()) {
        LOG_ERROR("Data not sent! (Data sent: %" PRIu32 "B, msg length: %zuB)", dataSent, sendMsg->length());
    }

    delete sendMsg;
    return true;
}

bool WorkerDesktop::handleServiceQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    if (!initialized) {
        return false;
    }
    auto &serviceQueue = getServiceQueue();

    sys::WorkerCommand cmd;
    if (serviceQueue.Dequeue(&cmd, 0)) {
        LOG_DEBUG("Received command: %d", static_cast<int>(cmd.command));
#if defined(DEBUG)
        assert(false);
#endif
    }
    else {
        LOG_ERROR("Failed dequeue for '%s'.", SERVICE_QUEUE_NAME.c_str());
        return false;
    }
    return true;
}

bool WorkerDesktop::handleIrqQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    bsp::USBDeviceStatus notification = bsp::USBDeviceStatus::Disconnected;
    if (!queue->Dequeue(&notification, 0)) {
        LOG_ERROR("Failed dequeue for %s.", sdesktop::irqQueueName);
        return false;
    }

    switch (notification) {
    case bsp::USBDeviceStatus::Connected:
        LOG_DEBUG("USB status: Connected");
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConnected>(),
                                        sys::BusChannel::USBNotifications);
        break;

    case bsp::USBDeviceStatus::Configured:
        LOG_DEBUG("USB status: Configured");
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConfigured>(),
                                        sys::BusChannel::USBNotifications);
        configured = true;
        break;

    case bsp::USBDeviceStatus::Disconnected:
        LOG_DEBUG("USB status: Disconnected");
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBDisconnected>(),
                                        sys::BusChannel::USBNotifications);
        configured = false;
        break;

    case bsp::USBDeviceStatus::DataReceived:
        bsp::usbHandleDataReceived();
        break;

    case bsp::USBDeviceStatus::Reset:
        LOG_DEBUG("USB status: Reset");
        if (configured) {
            reset();
        }
        break;

    default:
        LOG_ERROR("Notification not valid.");
        return false;
    }

    return true;
}

bool WorkerDesktop::handleSignallingQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    if (!initialized) {
        return false;
    }

    sys::WorkerCommand cmd;
    if (!queue->Dequeue(&cmd, 0)) {
        LOG_ERROR("Failed dequeue for '%s'.", sdesktop::signallingQueueName);
        return false;
    }

    switch (static_cast<Signal>(cmd.command)) {
    case Signal::unlockMTP:
        bsp::usbUnlockMTP();
        break;
    default:
        LOG_ERROR("Command not valid.");
        return false;
    }
    return true;
}
