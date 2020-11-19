﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string.h>
#include <stdio.h>
#include <filesystem>
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "Service/Timer.hpp"
#include "parser/ParserFSM.hpp"
#include "bsp/usb/usb.hpp"

class WorkerDesktop : public sys::Worker, public bsp::USBDeviceListener
{
  public:
    enum TransferFailAction
    {
        doNothing,
        removeDesitnationFile
    };
    WorkerDesktop(sys::Service *ownerServicePtr);

    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool handleMessage(uint32_t queueID) override final;

    sys::Service *ownerService = nullptr;
    parserFSM::StateMachine parser;

    xQueueHandle getReceiveQueue()
    {
        return receiveQueue;
    }
    sys::ReturnCodes startDownload(const std::filesystem::path &destinationPath, const uint32_t fileSize);
    void stopTransfer(const TransferFailAction action);

    void timerHandler(void);

    void rawDataReceived(void *dataPtr, uint32_t dataLen) override;
    bool getRawMode() const noexcept override;

  private:
    void uploadFileFailedResponse();
    xQueueHandle receiveQueue;
    FILE *fileDes                  = nullptr;
    uint32_t writeFileSizeExpected = 0;
    uint32_t writeFileDataWritten  = 0;
    std::filesystem::path filePath;
    volatile bool rawModeEnabled = false;
    std::unique_ptr<sys::Timer> transferTimer;
};
