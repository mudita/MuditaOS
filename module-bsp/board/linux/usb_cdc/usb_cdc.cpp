// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/usb/usb.hpp"
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <string>

namespace bsp
{

#if USBCDC_ECHO_ENABLED
    bool usbCdcEchoEnabled = false;

    constexpr std::string_view usbCDCEchoOnCmd("UsbCdcEcho=ON");
    constexpr std::string_view usbCDCEchoOffCmd("UsbCdcEcho=OFF");

    constexpr auto usbCDCEchoOnCmdLength  = usbCDCEchoOnCmd.length();
    constexpr auto usbCDCEchoOffCmdLength = usbCDCEchoOffCmd.length();
#endif

    void usbDeviceTask(void *ptr)
    {
        usbCDCReceive(ptr);
    }

    int usbCDCReceive(void *)
    {
        return 0;
    }

    int usbCDCSend(std::string *sendMsg)
    {
        return 0;
    }

    int usbCDCSendRaw(const char *dataPtr, size_t dataLen)
    {
        return 0;
    }

    void writePtsToFile(const char *pts_name)
    {
    }

    void usbDeinit()
    {
    }

    int usbInit(xQueueHandle, USBDeviceListener *)
    {
        return 0;
    }
} // namespace bsp
