// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Worker.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <service-desktop/parser/ParserFSM.hpp>
#include <bsp/usb/usb.hpp>
#include <service-desktop/USBSecurityModel.hpp>

#include <crc32.h>

#include <filesystem>
#include <atomic>

class WorkerDesktop : public sys::Worker
{
  public:
    enum class Signal
    {
        unlockMTP
    };

    WorkerDesktop(sys::Service *ownerServicePtr,
                  std::function<void()> messageProcessedCallback,
                  const sdesktop::USBSecurityModel &securityModel,
                  const std::string &serialNumber,
                  const std::string &caseColour,
                  const std::string &rootPath);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    void closeWorker();

    bool handleMessage(std::uint32_t queueID) override final;
    void notify(Signal command);

  private:
    void reset();

    bool handleReceiveQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleSendQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleServiceQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleIrqQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);
    bool handleSignallingQueueMessage(std::shared_ptr<sys::WorkerQueue> &queue);

    std::atomic<bool> initialized = false;
    std::atomic<bool> configured  = false;

    xQueueHandle receiveQueue;
    xQueueHandle irqQueue;
    const sdesktop::USBSecurityModel &securityModel;
    const std::string serialNumber;
    const std::string caseColour;
    std::string rootPath;
    sys::Service *ownerService = nullptr;
    sdesktop::endpoints::StateMachine parser;

    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
    std::function<void()> messageProcessedCallback;
};
