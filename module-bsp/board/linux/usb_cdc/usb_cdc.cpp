// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/usb/usb.hpp"
#include <termios.h>
#include <fcntl.h>
#include <fstream>
#include <string>

namespace bsp
{
    int fd;
    xQueueHandle USBReceiveQueue;
    xQueueHandle USBIrqQueue;
    constexpr auto ptsFileName = "/tmp/purephone_pts_name";
    char *pts_name             = NULL;
    bool shouldExecute         = false;

#if USBCDC_ECHO_ENABLED
    bool usbCdcEchoEnabled = false;

    constexpr std::string_view usbCDCEchoOnCmd("UsbCdcEcho=ON");
    constexpr std::string_view usbCDCEchoOffCmd("UsbCdcEcho=OFF");

    constexpr auto usbCDCEchoOnCmdLength  = usbCDCEchoOnCmd.length();
    constexpr auto usbCDCEchoOffCmdLength = usbCDCEchoOffCmd.length();
#endif

    static void usbCDCReceive(void *)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading on fd:%d", fd);
        char inputData[SERIAL_BUFFER_LEN];

        while (shouldExecute) {
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
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
            else {
                LOG_DEBUG("[ServiceDesktop:BSP_Driver] USB receive Queue is full, yielding task");
                vTaskDelay(1000);
            }
        }
    }

    void usbDeviceTask(void *ptr)
    {
        usbCDCReceive(ptr);
        vTaskDelete(nullptr);
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

    void writePtsToFile(const char *name)
    {
        std::ofstream ptsNameFile;
        ptsNameFile.open(ptsFileName, std::ios::out | std::ios::trunc);
        ptsNameFile << name;
    }

    void usbDeinit()
    {
        shouldExecute = false;
        LOG_INFO("usbDeinit removing file %s", ptsFileName);
        std::remove(ptsFileName);
    }

    void usbReinit(const std::string &)
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
        shouldExecute   = true;

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

    void usbHandleDataReceived()
    {}
} // namespace bsp
