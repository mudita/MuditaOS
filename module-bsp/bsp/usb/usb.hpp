// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

#define SERIAL_TRANSMISSION_START 0x02
#define SERIAL_TRANSMISSION_END 0x03
#define SERIAL_SHELL_START 0x33
#define SERIAL_BAUDRATE 115200
#define SERIAL_BUFFER_LEN 512 // this matches the buffer length in rt1051 cdc implementaion

namespace bsp
{
    int usbInit(xQueueHandle);
    void usbCDCReceive(void *ptr);
    int usbCDCSend(std::string *sendMsg);
} // namespace bsp
