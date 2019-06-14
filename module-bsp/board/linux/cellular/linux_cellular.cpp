
/*
 * @file linux_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "linux_cellular.hpp"

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
            set_interface_attribs();
            set_mincount(0);

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

    uint32_t LinuxCellular::Read(void *buf, size_t nbytes) {
        return read(fd,buf,nbytes);
    }

    uint32_t LinuxCellular::Write(void *buf, size_t nbytes) {
        return write(fd,buf,nbytes);
    }

    uint32_t LinuxCellular::Wait(uint32_t timeout) {
        auto event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
        if(event_count == 0){
            return 0; // timeout
        }
        else{
            return 1;
        }


    }


}