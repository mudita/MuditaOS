// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
#include <vector>
#include <filesystem>
#include <service-desktop/DesktopMessages.hpp>
#include "system/messages/SentinelRegistrationMessage.hpp"

inline constexpr auto uploadFailedMessage = "file upload terminated before all data transferred";

WorkerDesktop::WorkerDesktop(sys::Service *ownerServicePtr,
                             const sdesktop::USBSecurityModel &securityModel,
                             const std::string serialNumber)
    : sys::Worker(ownerServicePtr, sdesktop::worker_stack), securityModel(securityModel), serialNumber(serialNumber),
      ownerService(ownerServicePtr), parser(ownerServicePtr)
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

    usbSuspendTimer = sys::TimerFactory::createSingleShotTimer(
        ownerService, "usbSuspend", constants::usbSuspendTimeout, [this](sys::Timer &) { suspendUsb(); });

    bsp::usbInitParams initParams = {receiveQueue, irqQueue, serialNumber.c_str()};

    initialized = (bsp::usbInit(initParams) < 0) ? false : true;

    return initialized;
}

bool WorkerDesktop::deinit(void)
{
    if (!initialized) {
        return false;
    }

    unsigned int maxcount = 10;
    while (parser.getCurrentState() != sdesktop::endpoints::State::NoMsg && --maxcount > 0) {
        vTaskDelay(300);
    }
    LOG_INFO("we can deinit worker now");

    /// additional wait to flush on serial - we should wait for data sent...
    vTaskDelay(3000);

    bsp::usbDeinit();

    Worker::deinit();

    LOG_DEBUG("deinit end");
    initialized = false;

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

        using namespace sdesktop::endpoints;
        auto factory = std::make_unique<SecuredEndpointFactory>(securityModel.getEndpointSecurity());
        auto handler = std::make_unique<MessageHandler>(ownerService, std::move(factory));

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

void WorkerDesktop::suspendUsb()
{
    bsp::usbSuspend();
}
