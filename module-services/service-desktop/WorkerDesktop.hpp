// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

class WorkerDesktop : public sys::Worker
{
  public:
    WorkerDesktop(sys::Service *ownerServicePtr,
                  std::function<void()> messageProcessedCallback,
                  const sdesktop::USBSecurityModel &securityModel,
                  const std::string &serialNumber);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    void closeWorker();
    bool reinit(const std::filesystem::path &path);

    bool handleMessage(std::uint32_t queueID) override final;

    xQueueHandle getReceiveQueue()
    {
        return receiveQueue;
    }

  private:
    void reset();
    void suspendUsb();

    bool handleReceiveQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleSendQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleServiceQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleIrqQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);

    std::atomic<bool> initialized = false;

    xQueueHandle receiveQueue;
    xQueueHandle irqQueue;
    const sdesktop::USBSecurityModel &securityModel;
    const std::string serialNumber;
    sys::Service *ownerService = nullptr;
    sdesktop::endpoints::StateMachine parser;
    bsp::USBDeviceStatus usbStatus = bsp::USBDeviceStatus::Disconnected;

    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    std::function<void()> messageProcessedCallback;
};
