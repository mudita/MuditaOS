
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

#include "log/log.hpp"
#include "mutex.hpp"
#include <common_data/EventStore.hpp>
#include <fcntl.h>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <ticks.hpp>
#include <time.h>
#include <unistd.h>

#define _LINUX_UART_DEBUG   0

namespace bsp {

    std::map<uint32_t, speed_t> PortSpeeds_text = { {9600U, B9600}, {19200U, B19200}, {38400U, B38400}, {57600U, B57600}, {115200U, B115200}, {230400U, B230400}, {460800U, B460800} };

    LinuxCellular::LinuxCellular(const char *term, uint32_t portSpeed) {

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
            speed_t speed = PortSpeeds_text[portSpeed];//B115200;
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
            LOG_FATAL("Failed to add file descriptor to epoll");
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

    void LinuxCellular::Restart() {

    }

    ssize_t LinuxCellular::Read(void *buf, size_t nbytes) {

        cpp_freertos::LockGuard lock(serOutMutex);
        retry:
        auto ret = read(fd, buf, nbytes);
        if ((ret == -1) && (errno == EINTR)) {
            goto retry;
        }
        #if _LINUX_UART_DEBUG
        if (ret > 0) {
            LOG_PRINTF("[RX] ");
            uint8_t *ptr = (uint8_t*)buf;
            for (size_t i = 0; i < ret; i++)
                LOG_PRINTF("%02X ", (uint8_t)*ptr++);
            LOG_PRINTF("\n");
        }
        #endif
        return ret;
    }

    ssize_t LinuxCellular::Write(void *buf, size_t nbytes) {
        cpp_freertos::LockGuard lock(serOutMutex);
        #if _LINUX_UART_DEBUG
        LOG_PRINTF("[TX] ");
        uint8_t *ptr = (uint8_t*)buf;
        for (size_t i = 0; i < nbytes; i++)
            LOG_PRINTF("%02X ", (uint8_t)*ptr++);
        LOG_PRINTF("\n");
        #endif
        retry:
        auto ret = write(fd, buf, nbytes);
        if((ret == -1) && (errno == EINTR)){
            goto retry;
        }
        return ret;
    }

    void LinuxCellular::InformModemHostAsleep(void){

    }

    void LinuxCellular::InformModemHostWakeup(void){

    }

    void LinuxCellular::EnterSleep() {

    }

    void LinuxCellular::ExitSleep() {

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

    void LinuxCellular::SetSpeed(uint32_t portSpeed) {
        struct termios t;
        memset(&t, 0, sizeof(t));
        tcgetattr(fd, &t);
        cfmakeraw(&t);
        t.c_cflag |= CLOCAL;
        //if(ctsrts == 1)
        //  t.c_cflag |= CRTSCTS; //enable the flow control on dev board
        //else
        t.c_cflag &= ~(CRTSCTS);   //disable the flow control on dev board
        speed_t speed = PortSpeeds_text[portSpeed]; //B115200;
        cfsetispeed(&t, speed);
        cfsetospeed(&t, speed);
        tcsetattr(fd, TCSANOW, &t);
        int status = TIOCM_DTR | TIOCM_RTS;
        ioctl(fd, TIOCMBIS, &status);
    }

    namespace cellular
    {
        void Init(xQueueHandle qHandle)
        {
        }
        int SIMTrayStatus()
        {
            return 1;
        }
    }; // namespace cellular
}
