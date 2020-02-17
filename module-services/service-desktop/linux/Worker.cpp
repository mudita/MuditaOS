#include "Worker.hpp"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <log/log.hpp>
#include <queue.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

extern int errno;

extern xQueueHandle USBReceiveQueue;

namespace bsp
{

    int fd;

    void desktopServiceReceive(void *)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading on fd:%d", fd);
        uint8_t inputData[SERIAL_BUFFER_LEN];
        static std::string receive_msg;

        while (1)
        {
            if (uxQueueSpacesAvailable(USBReceiveQueue) != 0)
            {
                ssize_t length = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
                if (length > 0)
                {
                    receive_msg = std::string(inputData, inputData + length);
                    LOG_DEBUG("[ServiceDesktop:BSP_Driver] Received: %d signs", length);
                    xQueueSend(USBReceiveQueue, &receive_msg, portMAX_DELAY);
                }
                else
                {
                    // yielding task because nothing in a buffer
                    vTaskDelay(10);
                }
            }
            else
            {
                LOG_DEBUG("[ServiceDesktop:BSP_Driver] USB receive Queue is full, yielding task");
                vTaskDelay(1000);
            }
        }
    }

    int desktopServiceSend(std::string *send_msg)
    {
        ssize_t t = write(fd, (*send_msg).c_str(), (*send_msg).length());
        delete send_msg;

        if (t >= 0)
        {
            LOG_DEBUG("[ServiceDesktop:BSP_Driver] Send: %d signs", t);
            return 0;
        }
        else
        {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Writing to PTY failed with code: %d", errno);
            return -1;
        }
    }

    int desktopServiceInit()
    {

        fd = 0;
        fd = open("/dev/ptmx", O_RDWR | O_NOCTTY);

        if (fd == -1)
        {
            return (-1);
        }

        grantpt(fd);
        unlockpt(fd);

        char *pts_name = ptsname(fd);
        LOG_INFO("[ServiceDesktop:BSP_Driver] VCOMAPPInit linux ptsname: %s", pts_name);
        struct termios newtio;
        memset(&newtio, 0, sizeof(newtio));
        struct termios oldtio;
        tcgetattr(fd, &oldtio);

        newtio = oldtio;
        newtio.c_cflag = SERIAL_BAUDRATE | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = 0;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;
        newtio.c_cc[VMIN] = 1;
        newtio.c_cc[VTIME] = 0;
        tcflush(fd, TCIFLUSH);

        cfsetispeed(&newtio, SERIAL_BAUDRATE);
        cfsetospeed(&newtio, SERIAL_BAUDRATE);
        tcsetattr(fd, TCSANOW, &newtio);

        xTaskHandle taskHandleReceive;

        BaseType_t task_error = xTaskCreate(desktopServiceReceive, "USB_Linux_Receive", SERIAL_BUFFER_LEN * 8, (void *)1, tskIDLE_PRIORITY, &taskHandleReceive);

        if (task_error != pdPASS)
        {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Failed to start freertos USB_Linux_Receive");
        }

        return (fd);
    }
} // namespace bsp
