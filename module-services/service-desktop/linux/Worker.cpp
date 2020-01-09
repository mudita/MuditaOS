#include "Worker.hpp"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <log/log.hpp>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
extern int errno;

void desktop_receive(void *pointer)
{
    ServiceDesktop *owner = (ServiceDesktop *)pointer;

    int fd = owner->getPtyDescriptor();
    LOG_INFO("start reading on fd:%d", owner->getPtyDescriptor());
    static uint8_t inputData[SERIAL_BUFFER_LEN];
    while (1)
    {
        ssize_t ret = read(fd, &inputData[0], SERIAL_BUFFER_LEN);
        if (ret > 0)
        {
            owner->dataReceived(inputData, ret);
        }
        else if (ret == -1)
        {
            LOG_ERROR("reading from PTY resulted in an error %d", errno);
        }
    }

    LOG_INFO("exit");
}

int desktop_send(int fd, uint8_t *data, size_t dataSize)
{
    size_t t = write(fd, data, dataSize);
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

int desktop_init()
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
