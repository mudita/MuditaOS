
/*
 * @file linux_cellular.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_LINUX_CELLULAR_HPP
#define PUREPHONE_LINUX_CELLULAR_HPP

#include "cellular/bsp_cellular.hpp"


#include <cstring>
#include <sys/epoll.h>
#include "termios.h"
#include <sys/ioctl.h>


namespace bsp {

    class LinuxCellular: public Cellular  {

    public:

        LinuxCellular(const char* term);
        ~LinuxCellular();

        uint32_t Wait(uint32_t timeout) override final;

        uint32_t Read(void *buf, size_t nbytes) override final;

        uint32_t Write(void *buf, size_t nbytes) override final;

    private:

        int set_interface_attribs()
        {
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

        void set_mincount(int mcount)
        {
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


        static constexpr speed_t baud_bits[] = {
                0, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B921600, B1500000, B2000000, B3000000, B4000000
        };

        static const uint32_t portBaudRate = 115200;

        static const uint32_t MAX_EVENTS = 1;

        int fd;

        int epoll_fd;

        struct epoll_event event, events[MAX_EVENTS];


    };

}


#endif //PUREPHONE_LINUX_CELLULAR_HPP
