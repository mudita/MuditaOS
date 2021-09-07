// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/usb/usb.hpp"
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/inotify.h>
#include <array>
#include <limits.h>

#ifndef DEUBG_USB
#undef LOG_PRINTF
#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL
#undef LOG_CUSTOM

#define LOG_PRINTF(...)
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#define LOG_CUSTOM(loggerLevel, ...)
#endif
namespace bsp
{
    int fd;
    int fdNofity;
    xQueueHandle USBReceiveQueue;
    xQueueHandle USBIrqQueue;
    constexpr auto ptsFileName = "/tmp/purephone_pts_name";
    char *pts_name             = NULL;

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

    void checkUsbStatus()
    {
        char eventsBuff[((sizeof(inotify_event) + NAME_MAX + 1))];
        int len = read(fdNofity, eventsBuff, ((sizeof(inotify_event) + NAME_MAX + 1)));
        if (len > 0) {
            const inotify_event *event = (inotify_event *)&eventsBuff[0];
            if (event->mask & IN_OPEN) {
                USBDeviceStatus notification = USBDeviceStatus::Configured;
                xQueueSend(USBIrqQueue, &notification, 0);
            }
            if (event->mask & IN_CLOSE_WRITE) {
                USBDeviceStatus notification = USBDeviceStatus::Disconnected;
                xQueueSend(USBIrqQueue, &notification, 0);
            }
        }
    }

    int usbCDCReceive(void *)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading on fd:%d", fd);
        char inputData[SERIAL_BUFFER_LEN];

        while (1) {

            checkUsbStatus();

            if (uxQueueSpacesAvailable(USBReceiveQueue) != 0) {
                memset(inputData, 0, SERIAL_BUFFER_LEN);

                ssize_t length = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
                if (length > 0) {
                    LOG_DEBUG("[ServiceDesktop:BSP_Driver] Received: %d signs", static_cast<int>(length));
#if USBCDC_ECHO_ENABLED
                    bool usbCdcEchoEnabledPrev = usbCdcEchoEnabled;

                    auto usbEchoCmd = std::string_view{inputData, static_cast<size_t>(length)};

                    if ((length == usbCDCEchoOnCmdLength) && (usbCDCEchoOnCmd == usbEchoCmd)) {
                        usbCdcEchoEnabled = true;
                    }
                    else if ((length == usbCDCEchoOffCmdLength) && (usbCDCEchoOffCmd == usbEchoCmd)) {
                        usbCdcEchoEnabled = false;
                    }

                    if (usbCdcEchoEnabled || usbCdcEchoEnabledPrev) {
                        usbCDCSendRaw(inputData, length);
                        LOG_DEBUG(
                            "[ServiceDesktop:BSP_Driver] Echoed: %d signs: [%s]", static_cast<int>(length), inputData);
                        continue;
                    }
#endif
                    std::string *receiveMsg = new std::string(inputData, inputData + length);
                    xQueueSend(USBReceiveQueue, &receiveMsg, portMAX_DELAY);
                }
                else {
                    // yielding task because nothing in a buffer
                    vTaskDelay(10);
                }
            }
            else {
                LOG_DEBUG("[ServiceDesktop:BSP_Driver] USB receive Queue is full, yielding task");
                vTaskDelay(1000);
            }
        }
    }

    int usbCDCSend(std::string *sendMsg)
    {
        return usbCDCSendRaw(sendMsg->c_str(), sendMsg->length());
    }

    int usbCDCSendRaw(const char *dataPtr, size_t dataLen)
    {
        ssize_t t = write(fd, dataPtr, dataLen);
        if (t >= 0) {
            LOG_DEBUG("[ServiceDesktop:BSP_Driver] Sent: %d", static_cast<int>(t));
            return 0;
        }
        else {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Writing to PTY failed with code: %d", errno);
            return -1;
        }
    }

    void writePtsToFile(const char *pts_name)
    {
        std::ofstream ptsNameFile;
        ptsNameFile.open(ptsFileName, std::ios::out | std::ios::trunc);
        ptsNameFile << pts_name;
    }

    void usbDeinit()
    {
        LOG_INFO("usbDeinit removing file %s", ptsFileName);
        std::remove(ptsFileName);
    }

    void usbReinit(const char * /*mtpRoot*/)
    {
        LOG_INFO("usbReinit");
    }

    void usbSuspend()
    {
        LOG_INFO("usbSuspend");
    }

    int usbInit(const bsp::usbInitParams &initParams)
    {

        fd = 0;
        fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
        if (fd == -1) {
            LOG_ERROR("bsp::usbInit Failed to open /dev/ptmx, can't allocate new pseudo terminal");
            return -1;
        }

        grantpt(fd);
        unlockpt(fd);

        pts_name = ptsname(fd);
        if (pts_name == nullptr) {
            LOG_ERROR("bsp::usbInit ptsname returned NULL, no pseudo terminal allocated");
            return -1;
        }

        fdNofity = inotify_init1(O_NONBLOCK);
        inotify_add_watch(fdNofity, pts_name, IN_OPEN | IN_CLOSE_WRITE);

        writePtsToFile(pts_name);
        LOG_INFO("bsp::usbInit linux ptsname: %s", pts_name);
        struct termios newtio;
        memset(&newtio, 0, sizeof(newtio));
        struct termios oldtio;
        tcgetattr(fd, &oldtio);

        newtio             = oldtio;
        newtio.c_cflag     = SERIAL_BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag     = 0;
        newtio.c_oflag     = 0;
        newtio.c_lflag     = 0;
        newtio.c_cc[VMIN]  = 1;
        newtio.c_cc[VTIME] = 0;
        tcflush(fd, TCIFLUSH);

        cfsetispeed(&newtio, SERIAL_BAUDRATE);
        cfsetospeed(&newtio, SERIAL_BAUDRATE);
        tcsetattr(fd, TCSANOW, &newtio);

        xTaskHandle taskHandleReceive;
        USBReceiveQueue = initParams.queueHandle;
        USBIrqQueue     = initParams.irqQueueHandle;

        BaseType_t task_error = xTaskCreate(&bsp::usbDeviceTask,
                                            "USBLinuxReceive",
                                            SERIAL_BUFFER_LEN * 8,
                                            nullptr,
                                            tskIDLE_PRIORITY,
                                            &taskHandleReceive);

        if (task_error != pdPASS) {
            LOG_ERROR("bsp::usbInit Failed to start freertos USB_Linux_Receive");
            return -1;
        }

        return 0;
    }
} // namespace bsp
