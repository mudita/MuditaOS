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

#define SERIAL_BUFFER_LEN 512 // this matches the buffer length in rt1051 cdc implementaion
#define SERIAL_BAUDRATE 115200
namespace bsp
{
    class USBDeviceListener {
      public:
        virtual bool getRawMode() {
            return false;
        }
        virtual void rawDataReceived(void *dataPtr, uint32_t dataLen) = 0;
    };

    int usbInit(xQueueHandle, USBDeviceListener *deviceListener = nullptr);
    int usbCDCReceive(void *ptr);
    int usbCDCSend(std::string *sendMsg);
} // namespace bsp