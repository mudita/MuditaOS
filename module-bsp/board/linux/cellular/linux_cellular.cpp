
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#include "log/log.hpp"


namespace bsp
{

    LinuxCellular::LinuxCellular(const char* term) {

        if(strcmp(term,"0") == 0){
            fd = 0;
        }
        else{
            // open serial port
            fd = open(term,O_RDWR | O_NOCTTY | O_NONBLOCK);

            // Set serial port attributes
            //set_interface_attribs();
            //set_mincount(0);

        }

        epoll_fd = epoll_create1(0);

        if(epoll_fd == -1)
        {
            LOG_FATAL("Failed to create epoll file descriptor");
        }

        struct epoll_event event;

        event.events = EPOLLIN;
        event.data.fd = fd;

        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event))
        {
            LOG_FATAL("Failed to add file descriptor to epoll\n");
            close(epoll_fd);
        }

    }

    LinuxCellular::~LinuxCellular() {

        close(fd);

        if(close(epoll_fd))
        {
            LOG_FATAL("Failed to close epoll file descriptor");
        }
    }

    void LinuxCellular::PowerUp() {

    }

    void LinuxCellular::PowerDown() {

    }

    ssize_t LinuxCellular::Read(void *buf, size_t nbytes) {
        auto ret = read(fd,buf,nbytes);
        if(ret == -1){
            std::cout << "Read returned error: " << strerror(errno) << "\n";
        }
        return ret;
    }

    uint32_t LinuxCellular::Write(void *buf, size_t nbytes) {
        return write(fd,buf,nbytes);
    }

    uint32_t LinuxCellular::Wait(uint32_t timeout) {

        retry:
        auto event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
        if(event_count == 0){
            return 0; // timeout
        }
        else if((event_count == -1) && (errno == EINTR)){
            goto retry;
        }
        else{
            return 1;
        }


    }

    int LinuxCellular::set_interface_attribs() {
        struct termios tty;

        if (tcgetattr(fd, &tty) < 0) {
            printf("Error from tcgetattr: %s\n", strerror(errno));
            return -1;
        }

        cfsetospeed(&tty, (speed_t)portBaudRate);
        cfsetispeed(&tty, (speed_t)portBaudRate);

        tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;         /* 8-bit characters */
        tty.c_cflag &= ~PARENB;     /* no parity bit */
        tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
        tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

        /* setup for non-canonical mode */
        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tty.c_oflag &= ~OPOST;

        /* fetch bytes as they become available */
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 1;

        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            printf("Error from tcsetattr: %s\n", strerror(errno));
            return -1;
        }
        return 0;
    }

    void LinuxCellular::set_mincount(int mcount) {
        struct termios tty;

        if (tcgetattr(fd, &tty) < 0) {
            printf("Error tcgetattr: %s\n", strerror(errno));
            return;
        }

        tty.c_cc[VMIN] = mcount ? 1 : 0;
        tty.c_cc[VTIME] = 5;        /* half second timer */

        if (tcsetattr(fd, TCSANOW, &tty) < 0)
            printf("Error tcsetattr: %s\n", strerror(errno));
    }

}