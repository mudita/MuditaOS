// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include <endpoints/EndpointFactory.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <endpoints/message/Sender.hpp>
#include <MessageHandler.hpp>
#include "Timers/TimerFactory.hpp"

#include <bsp/usb/usb.hpp>
#include <log/log.hpp>
#include <crc32.h>

#include <map>
#include <utility>
#include <vector>
#include <filesystem>
#include <service-desktop/DesktopMessages.hpp>
#include "system/messages/SentinelRegistrationMessage.hpp"

inline constexpr auto uploadFailedMessage = "file upload terminated before all data transferred";

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr,
                             std::function<void()> messageProcessedCallback,
                             const sdesktop::USBSecurityModel &securityModel,
                             const std::string &serialNumber)
    : sys::Worker(ownerServicePtr, sdesktop::worker_stack), securityModel(securityModel), serialNumber(serialNumber),
      ownerService(ownerServicePtr), parser(ownerServicePtr),
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

    bsp::usbInitParams initParams = {receiveQueue, irqQueue, serialNumber.c_str()};

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
    LOG_INFO("we can deinit worker now");

    /// additional wait to flush on serial - we should wait for data sent...
    vTaskDelay(3000);

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
    initialized = false;
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

        using namespace sdesktop::endpoints;
        auto factory = EndpointFactory::create(securityModel.getEndpointSecurity());
        auto handler = std::make_unique<MessageHandler>(ownerService, messageProcessedCallback, std::move(factory));

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
            LOG_DEBUG("Received cmd: %d", static_cast<int>(cmd.command));
#if defined(DEBUG)
            assert(false);
#endif
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
            ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBConnected>(),
                                            sys::BusChannel::USBNotifications);
            usbStatus = bsp::USBDeviceStatus::Connected;
        }
        else if (notification == bsp::USBDeviceStatus::Configured) {
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
            usbStatus = bsp::USBDeviceStatus::Configured;
        }
        else if (notification == bsp::USBDeviceStatus::Disconnected) {
            ownerService->bus.sendMulticast(std::make_shared<sdesktop::usb::USBDisconnected>(),
                                            sys::BusChannel::USBNotifications);
            usbStatus = bsp::USBDeviceStatus::Disconnected;
        }
        else if (notification == bsp::USBDeviceStatus::DataReceived) {
            bsp::usbHandleDataReceived();
        }
    }
    else {
        LOG_INFO("handeMessage got message on an unhandled queue");
    }

    return true;
}

void WorkerDesktop::suspendUsb()
{
    bsp::usbSuspend();
}
