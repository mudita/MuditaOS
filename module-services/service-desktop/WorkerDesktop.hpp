// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <ParserFSM.hpp>
#include <bsp/usb/usb.hpp>
#include <service-desktop/USBSecurityModel.hpp>

#include <crc32.h>

#include <filesystem>
#include <atomic>

namespace constants
{
    constexpr auto usbSuspendTimeout = std::chrono::seconds{1};
} // namespace constants

class WorkerDesktop : public sys::Worker
{
  public:
    WorkerDesktop(sys::Service *ownerServicePtr,
                  const sdesktop::USBSecurityModel &securityModel,
                  const std::string serialNumber);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool reinit(const std::filesystem::path &path);

    bool handleMessage(uint32_t queueID) override final;

    xQueueHandle getReceiveQueue()
    {
        return receiveQueue;
    }

  private:
    void suspendUsb();

    bool stateChangeWait();
    bool initialized = false;
    xQueueHandle receiveQueue;
    xQueueHandle irqQueue;
    const sdesktop::USBSecurityModel &securityModel;
    const std::string serialNumber;
    sys::Service *ownerService = nullptr;
    sdesktop::endpoints::StateMachine parser;
    sys::TimerHandle usbSuspendTimer;
    bsp::USBDeviceStatus usbStatus = bsp::USBDeviceStatus::Disconnected;

    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
};
