
/*
 * @file linux_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "linux_cellular.hpp"

#include <iostream>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <ticks.hpp>


#include "log/log.hpp"


namespace bsp {

    LinuxCellular::LinuxCellular(const char *term) {

        if (strcmp(term, "0") == 0) {
            fd = 0;
        } else {
            // open serial port
            fd = open(term, O_RDWR | O_NOCTTY | O_NONBLOCK);
            if (fd == -1) {
                LOG_FATAL("Failed to open serial port");
                return;
            }

            struct termios t;
            memset(&t, 0, sizeof(t));
            tcgetattr(fd, &t);
            cfmakeraw(&t);
            t.c_cflag |= CLOCAL;
            //if(ctsrts == 1)
            //  t.c_cflag |= CRTSCTS; //enable the flow control on dev board
            //else
            t.c_cflag &= ~(CRTSCTS);//disable the flow control on dev board
            speed_t speed = B115200;
            cfsetispeed(&t, speed);
            cfsetospeed(&t, speed);
            tcsetattr(fd, TCSANOW, &t);
            int status = TIOCM_DTR | TIOCM_RTS;
            ioctl(fd, TIOCMBIS, &status);
        }

        epoll_fd = epoll_create1(0);

        if (epoll_fd == -1) {
            LOG_FATAL("Failed to create epoll file descriptor");
        }

        struct epoll_event event;

        event.events = EPOLLIN;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            LOG_FATAL("Failed to add file descriptor to epoll\n");
        }

        isInitialized = true;

    }

    LinuxCellular::~LinuxCellular() {

        if(fd >= 0){
            close(fd);
        }

        if(epoll_fd >= 0){
            close(epoll_fd);
        }
    }

    void LinuxCellular::PowerUp() {

    }

    void LinuxCellular::PowerDown() {

    }

    ssize_t LinuxCellular::Read(void *buf, size_t nbytes) {

        retry:
        auto ret = read(fd, buf, nbytes);
        if ((ret == -1) && (errno == EINTR)) {
            goto retry;
        }
        return ret;
    }

    ssize_t LinuxCellular::Write(void *buf, size_t nbytes) {
        return write(fd, buf, nbytes);
    }

    uint32_t LinuxCellular::Wait(uint32_t timeout) {

        uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
        uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
        uint32_t timeElapsed = currentTime;

        retry:

        if (timeElapsed >= timeoutNeeded) {
            return 0; // timeout
        }

        auto event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();

        if (event_count == 0) {
            return 0; // timeout
        } else if ((event_count == -1) && (errno == EINTR)) {
            goto retry;
        } else {
            return 1;
        }


    }

}