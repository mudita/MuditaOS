// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/usb/usb.hpp"
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/inotify.h>
#include <limits.h>

#if DEBUG_USB == 0
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

namespace
{
    xTaskHandle taskHandleReceive;
}

namespace bsp
{
    int fd;
    int fdNotify;
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
        constexpr auto eventsBufferSize = sizeof(inotify_event) + NAME_MAX + 1;
        char eventsBuffer[eventsBufferSize];

        const auto len = read(fdNotify, eventsBuffer, sizeof(eventsBuffer));
        if (len <= 0) {
            return;
        }

        const auto event = reinterpret_cast<inotify_event *>(eventsBuffer);
        if (event->mask & IN_OPEN) {
            USBDeviceStatus notification = USBDeviceStatus::Configured;
            xQueueSend(USBIrqQueue, &notification, 0);
        }
        if (event->mask & IN_CLOSE_WRITE) {
            USBDeviceStatus notification = USBDeviceStatus::Disconnected;
            xQueueSend(USBIrqQueue, &notification, 0);
        }
    }

    ssize_t usbCDCReceive([[maybe_unused]] void *ptr)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading on fd:%d", fd);
        char inputData[constants::serial::bufferLength];

        while (true) {

            checkUsbStatus();

            if (uxQueueSpacesAvailable(USBReceiveQueue) != 0) {
                memset(inputData, 0, sizeof(inputData));

                const auto length = read(fd, inputData, sizeof(inputData));
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
                    auto receivedMsg = new std::string(inputData, inputData + length);
                    xQueueSend(USBReceiveQueue, &receivedMsg, portMAX_DELAY);
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

    std::size_t usbCDCSend(std::string *sendMsg)
    {
        return usbCDCSendRaw(sendMsg->c_str(), sendMsg->length());
    }

    std::size_t usbCDCSendRaw(const char *dataPtr, std::size_t dataLen)
    {
        const auto t = write(fd, dataPtr, dataLen);
        if (t < 0) {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Writing to PTY failed with code: %d", errno);
            return -1;
        }
        LOG_DEBUG("[ServiceDesktop:BSP_Driver] Sent: %dB", static_cast<int>(t));
        return 0;
    }

    void writePtsToFile(const char *ptsname)
    {
        std::ofstream ptsNameFile;
        ptsNameFile.open(ptsFileName, std::ios::out | std::ios::trunc);
        ptsNameFile << ptsname;
    }

    void usbDeinit()
    {
        LOG_INFO("usbDeinit removing file %s", ptsFileName);
        std::remove(ptsFileName);
        if (taskHandleReceive != nullptr) {
            vTaskDelete(taskHandleReceive);
        }
    }

    void usbUnlockMTP()
    {
        LOG_INFO("mtpUnlock");
    }

    int usbInit(const bsp::usbInitParams &initParams)
    {
        fd = 0;
        fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);
        if (fd == -1) {
            LOG_ERROR("Failed to open /dev/ptmx, can't allocate new pseudo terminal");
            return -1;
        }

        grantpt(fd);
        unlockpt(fd);

        pts_name = ptsname(fd);
        if (pts_name == nullptr) {
            LOG_ERROR("ptsname returned NULL, no pseudo terminal allocated");
            return -1;
        }

        fdNotify = inotify_init1(O_NONBLOCK);
        inotify_add_watch(fdNotify, pts_name, IN_OPEN | IN_CLOSE_WRITE);

        writePtsToFile(pts_name);
        LOG_INFO("linux ptsname: %s", pts_name);
        struct termios newtio
        {};
        memset(&newtio, 0, sizeof(newtio));
        struct termios oldtio
        {};
        tcgetattr(fd, &oldtio);

        newtio             = oldtio;
        newtio.c_cflag     = constants::serial::baudRate | CS8 | CLOCAL | CREAD;
        newtio.c_iflag     = 0;
        newtio.c_oflag     = 0;
        newtio.c_lflag     = 0;
        newtio.c_cc[VMIN]  = 1;
        newtio.c_cc[VTIME] = 0;
        tcflush(fd, TCIFLUSH);

        cfsetispeed(&newtio, constants::serial::baudRate);
        cfsetospeed(&newtio, constants::serial::baudRate);
        tcsetattr(fd, TCSANOW, &newtio);

        USBReceiveQueue = initParams.queueHandle;
        USBIrqQueue     = initParams.irqQueueHandle;

        const auto task_error = xTaskCreate(&bsp::usbDeviceTask,
                                            "USBLinuxReceive",
                                            constants::serial::bufferLength * 8,
                                            nullptr,
                                            tskIDLE_PRIORITY,
                                            &taskHandleReceive);

        if (task_error != pdPASS) {
            LOG_ERROR("Failed to start freertos USB_Linux_Receive");
            return -1;
        }

        return 0;
    }

    void usbHandleDataReceived()
    {}
} // namespace bsp
