#include "Worker.hpp"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <log/log.hpp>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <queue.h>

extern int errno;

extern xQueueHandle USBReceiveQueue;
extern xQueueHandle USBSendQueue;

int fd;

void desktopServiceReceive(void *)
{
    LOG_INFO("start reading on fd:%d", fd);
    static uint8_t inputData[SERIAL_BUFFER_LEN];

    while (1)
    {
        ssize_t ret = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
        if (ret > 0)
        {
            static std::string msg;
            msg = std::string(inputData, inputData + ret);
            LOG_DEBUG("Received: %d signs", ret);
            xQueueSend(USBReceiveQueue, &msg, portMAX_DELAY);
        }
    }
    LOG_INFO("exit");
}

int desktopServiceSend(std::string msg)
{

    ssize_t t = write(fd, msg.c_str(), msg.length());
    if (t >= 0)
    {
        return (t);
    }
    else
    {
        LOG_ERROR("writing to PTY failed with code: %d", errno);
        return (-1);
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
    LOG_INFO("VCOMAPPInit linux ptsname: %s", pts_name);
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
        LOG_ERROR("failed to start freertos core task");
    }

    return (fd);
}
