// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include "service-desktop/DeviceColour.hpp"
#include <endpoints/EndpointFactory.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <endpoints/message/Sender.hpp>
#include <MessageHandler.hpp>
#include "Timers/TimerFactory.hpp"

#include <bsp/usb/usb.hpp>
#include <log/log.hpp>
#include <crc32.h>

#include <utility>
#include <vector>
#include <filesystem>
#include "system/messages/SentinelRegistrationMessage.hpp"

inline constexpr auto uploadFailedMessage = "file upload terminated before all data transferred";

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr,
                             std::function<void()> messageProcessedCallback,
                             const sdesktop::USBSecurityModel &securityModel,
                             const std::string &serialNumber,
                             const std::string &caseColour,
                             const std::string &rootPath)
    : sys::Worker(ownerServicePtr, sdesktop::worker_stack), securityModel(securityModel), serialNumber(serialNumber),
      caseColour(caseColour), rootPath{rootPath}, ownerService(ownerServicePtr), parser(ownerServicePtr),
      messageProcessedCallback(std::move(messageProcessedCallback))
{}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    if (initialized) {
        return true;
    }

    Worker::init(queues);

    irqQueue     = Worker::getQueueHandleByName(sdesktop::IRQ_QUEUE_BUFFER_NAME);
    receiveQueue = Worker::getQueueHandleByName(sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
    sdesktop::endpoints::sender::setSendQueueHandle(Worker::getQueueHandleByName(sdesktop::SEND_QUEUE_BUFFER_NAME));

    cpuSentinel                  = std::make_shared<sys::CpuSentinel>("WorkerDesktop", ownerService);
    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    ownerService->bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    bsp::usbInitParams initParams = {
        receiveQueue, irqQueue, serialNumber, device_colour::getColourVersion(caseColour), rootPath};

    initialized = bsp::usbInit(initParams) >= 0;

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
    LOG_INFO("we can deinit worker now");

    /// additional wait to flush on serial - we should wait for data sent...
    vTaskDelay(500);

    bsp::usbDeinit();

    this->close();

    cpuSentinel->ReleaseMinimumFrequency();
    auto sentinelRemoveMsg = std::make_shared<sys::SentinelRemovalMessage>("WorkerDesktop");
    auto result            = ownerService->bus.sendUnicastSync(sentinelRemoveMsg, service::name::system_manager, 1000);
    if (result.first != sys::ReturnCodes::Success) {
        LOG_ERROR("Sentinel %s could not be removed!", cpuSentinel->GetName().c_str());
    }
    cpuSentinel.reset();

    LOG_DEBUG("deinit end");
}

bool WorkerDesktop::reinit(const std::filesystem::path &path)
{
    LOG_DEBUG("Reinit USB begin");

    bsp::usbReinit(path);

    LOG_DEBUG("Reinit USB end");
    return true;
}

void WorkerDesktop::reset()
{
    initialized = false;
    usbStatus   = bsp::USBDeviceStatus::Disconnected;
    bsp::usbDeinit();

    bsp::usbInitParams initParams = {
        receiveQueue, irqQueue, serialNumber, device_colour::getColourVersion(caseColour), rootPath};
    initialized = bsp::usbInit(initParams) >= 0;
    if (initialized) {
        usbStatus = bsp::USBDeviceStatus::Connected;
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConnected>(),
                                        sys::BusChannel::USBNotifications);
    }
}

bool WorkerDesktop::handleMessage(std::uint32_t queueID)
{
    bool result       = false;
    auto &queue       = queues[queueID];
    const auto &qname = queue->GetQueueName();

    if (qname == sdesktop::RECEIVE_QUEUE_BUFFER_NAME) {
        result = handleReceiveQueueMessage(queue);
    }
    else if (qname == sdesktop::SEND_QUEUE_BUFFER_NAME) {
        result = handleSendQueueMessage(queue);
    }
    else if (qname == SERVICE_QUEUE_NAME) {
        result = handleServiceQueueMessage(queue);
    }
    else if (qname == sdesktop::IRQ_QUEUE_BUFFER_NAME) {
        result = handleIrqQueueMessage(queue);
    }
    else {
        LOG_INFO("handeMessage got message on an unhandled queue");
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
        LOG_ERROR("handleMessage failed to receive from \"%s\"", sdesktop::RECEIVE_QUEUE_BUFFER_NAME);
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
        LOG_ERROR("handleMessage xQueueReceive failed for %s.", sdesktop::SEND_QUEUE_BUFFER_NAME);
        return false;
    }

    bsp::usbCDCSend(sendMsg);
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
        LOG_DEBUG("Received cmd: %d", static_cast<int>(cmd.command));
#if defined(DEBUG)
        assert(false);
#endif
    }
    else {
        LOG_ERROR("handleMessage xQueueReceive failed for %s.", SERVICE_QUEUE_NAME.c_str());
        return false;
    }
    return true;
}
bool WorkerDesktop::handleIrqQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue)
{
    bsp::USBDeviceStatus notification = bsp::USBDeviceStatus::Disconnected;
    if (!queue->Dequeue(&notification, 0)) {
        LOG_ERROR("handleMessage xQueueReceive failed for %s.", sdesktop::IRQ_QUEUE_BUFFER_NAME);
        return false;
    }

    if (notification == bsp::USBDeviceStatus::Connected) {
        LOG_DEBUG("USB status: Connected");
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConnected>(),
                                        sys::BusChannel::USBNotifications);
        usbStatus = bsp::USBDeviceStatus::Connected;
    }
    else if (notification == bsp::USBDeviceStatus::Configured) {
        if (usbStatus == bsp::USBDeviceStatus::Connected) {
            LOG_DEBUG("USB status: First configuration");
            ownerService->bus.sendUnicast(
                std::make_shared<sdesktop::usb::USBConfigured>(sdesktop::usb::USBConfigurationType::firstConfiguration),
                service::name::service_desktop);
        }
        else {
            LOG_DEBUG("USB status: Reconfiguration");
            ownerService->bus.sendUnicast(
                std::make_shared<sdesktop::usb::USBConfigured>(sdesktop::usb::USBConfigurationType::reconfiguration),
                service::name::service_desktop);
        }
        usbStatus = bsp::USBDeviceStatus::Configured;
    }
    else if (notification == bsp::USBDeviceStatus::Disconnected) {
        LOG_DEBUG("USB status: Disconnected");
        ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBDisconnected>(),
                                        sys::BusChannel::USBNotifications);
        usbStatus = bsp::USBDeviceStatus::Disconnected;
    }
    else if (notification == bsp::USBDeviceStatus::DataReceived) {
        bsp::usbHandleDataReceived();
    }
    else if (notification == bsp::USBDeviceStatus::Reset) {
        LOG_DEBUG("USB status: Reset");
        if (usbStatus == bsp::USBDeviceStatus::Configured) {
            reset();
        }
    }
    return true;
}

void WorkerDesktop::suspendUsb()
{
    bsp::usbSuspend();
}
