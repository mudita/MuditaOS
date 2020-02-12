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
extern xQueueHandle queueHandleBuffer;

void desktopServiceReceive(void *ptr)
{
    ServiceDesktop *owner = (ServiceDesktop *)ptr;

    int fd = owner->getPtyDescriptor();
    LOG_INFO("start reading on fd:%d", owner->getPtyDescriptor());
    static uint8_t inputData[SERIAL_BUFFER_LEN];
    while (1)
    {
        ssize_t ret = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
        if (ret > 0)
        {
            LOG_DEBUG("Received: %d signs", ret);
            xQueueSend(queueHandleBuffer, inputData, portMAX_DELAY);
            owner->dataReceived(inputData, ret);
        }
    }

    LOG_INFO("exit");
}

void desktopServiceSend(void *ptr)
{
    ServiceDesktop *owner = (ServiceDesktop *)ptr;

    static uint8_t outputData[SERIAL_BUFFER_LEN];

    while (1)
    {
        if (xQueueReceive(queueHandleBuffer, outputData, portMAX_DELAY))
        {

            size_t t = write(owner->getPtyDescriptor(), outputData, sizeof(outputData));
            if (t >= 0)
            {
                LOG_DEBUG("Send: %d signs", t);
            }
            else
            {
                LOG_ERROR("writing to PTY failed with code: %d", errno);
            }

        }
    }
}

int desktopServiceInit()
{
    int fd = 0;

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
    return (fd);
}
