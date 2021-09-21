// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

inline constexpr auto SERIAL_BUFFER_LEN = 512;
inline constexpr auto SERIAL_BAUDRATE   = 115200;

namespace bsp
{
    enum class USBDeviceStatus : uint32_t
    {
        Disconnected,
        Connected,
        Configured
    };

    struct usbInitParams
    {
        xQueueHandle queueHandle;
        xQueueHandle irqQueueHandle;
        const char *serialNumber;
    };

    int usbInit(const usbInitParams &initParams);
    int usbCDCReceive(void *ptr);
    int usbCDCSend(std::string *sendMsg);
    int usbCDCSendRaw(const char *dataPtr, size_t dataLen);
    void usbDeinit();
    void usbReinit(const char *mtpRoot);
    void usbSuspend();

} // namespace bsp
