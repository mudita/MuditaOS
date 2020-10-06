#pragma once

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "event_groups.h"
#ifndef TARGET_Linux
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"
#endif
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

#ifdef TARGET_Linux
struct usb_cdc_vcom_struct_t;
#endif

namespace bsp
{
    usb_cdc_vcom_struct_t *usbInit();

    // TODO: refactor it, to fit more to existing codebase
    void usbCDCReceive(void *ptr);
    int usbCDCSend(std::string *sendMsg);
} // namespace bsp
