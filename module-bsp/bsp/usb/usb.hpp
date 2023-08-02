// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
}

#include <errno.h>
#include <iostream>
#include <log/log.hpp>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace bsp
{
    namespace constants::serial
    {
        inline constexpr auto bufferLength = 512;
        inline constexpr auto baudRate = 115200;
    }

    enum class USBDeviceStatus : std::uint32_t
    {
        Disconnected,
        Connected,
        Configured,
        DataReceived,
        Reset
    };

    struct usbInitParams
    {
        xQueueHandle queueHandle;
        xQueueHandle irqQueueHandle;
        std::string serialNumber;
        std::uint16_t deviceVersion;
        std::string rootPath;
        bool mtpLockedAtInit;
    };

    int usbInit(const usbInitParams &initParams);
    ssize_t usbCDCReceive(void *ptr);
    std::size_t usbCDCSend(std::string *sendMsg);
    std::size_t usbCDCSendRaw(const char *dataPtr, std::size_t dataLen);
    void usbDeinit();
    void usbHandleDataReceived();
    void usbUnlockMTP();
} // namespace bsp
