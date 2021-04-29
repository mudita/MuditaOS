// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>
#include <atomic>
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "Service/CpuSentinel.hpp"
#include "parser/ParserFSM.hpp"
#include "endpoints/EndpointFactory.hpp"
#include "bsp/usb/usb.hpp"
#include "USBSecurityModel.hpp"

namespace constants
{
    constexpr auto usbSuspendTimeout = std::chrono::seconds{1};
} // namespace constants

class WorkerDesktop : public sys::Worker, public bsp::USBDeviceListener
{
  public:
    enum TransferFailAction
    {
        doNothing,
        removeDesitnationFile
    };
    enum class Command
    {
        CancelTransfer,
    };

    WorkerDesktop(sys::Service *ownerServicePtr, const sdesktop::USBSecurityModel &securityModel);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool reinit(const std::filesystem::path &path);

    bool handleMessage(uint32_t queueID) override final;

    xQueueHandle getReceiveQueue()
    {
        return receiveQueue;
    }

    sys::ReturnCodes startDownload(const std::filesystem::path &destinationPath, const uint32_t fileSize);
    void stopTransfer(const TransferFailAction action);

    void cancelTransferOnTimeout();

    void rawDataReceived(void *dataPtr, uint32_t dataLen) override;
    bool getRawMode() const noexcept override;

  private:
    void uploadFileFailedResponse();

    void transferTimeoutHandler();

    void startTransferTimer();
    void stopTransferTimer();
    void reloadTransferTimer();
    void suspendUsb();

    bool stateChangeWait();

    xQueueHandle receiveQueue;
    xQueueHandle irqQueue;
    FILE *fileDes                  = nullptr;
    uint32_t writeFileSizeExpected = 0;
    uint32_t writeFileDataWritten  = 0;
    std::filesystem::path filePath;
    std::atomic<bool> rawModeEnabled = false;
    const sdesktop::USBSecurityModel &securityModel;
    sys::Service *ownerService = nullptr;
    parserFSM::StateMachine parser;
    sys::TimerHandle usbSuspendTimer;
    bsp::USBDeviceStatus usbStatus = bsp::USBDeviceStatus::Disconnected;

    std::shared_ptr<sys::CpuSentinel> cpuSentinel;
};
