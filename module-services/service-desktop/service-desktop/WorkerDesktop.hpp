// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string.h>
#include <module-os/RTOSWrapper/include/ticks.hpp>
#include <module-os/RTOSWrapper/include/timer.hpp>
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "parser/ParserFSM.hpp"
#include "bsp/usb/usb.hpp"
#include "vfs.hpp"

class WorkerDesktop : public sys::Worker, public cpp_freertos::Timer, public bsp::USBDeviceListener
{
  public:
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
    sys::ReturnCodes startDownload(const fs::path &destinationPath, const uint32_t fileSize);
    void stopTransfer(const bool removeDestinationFile=false);
    void Run();

    void rawDataReceived(void *dataPtr, uint32_t dataLen) override;
    bool getRawMode() override;

    xQueueHandle receiveQueue;
    vfs::FILE *fileDes = nullptr;
    uint32_t writeFileSizeExpected = 0;
    uint32_t writeFileDataWritten  = 0;
    fs::path filePath;
    TaskHandle_t usbTaskHandle = NULL;
    volatile bool rawModeEnabled = false;
};
