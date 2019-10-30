
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

#include "bsp/cellular/bsp_cellular.hpp"


#include <cstring>
#include <sys/epoll.h>
#include "termios.h"
#include <sys/ioctl.h>
#include "mutex.hpp"

namespace bsp {

    class LinuxCellular: public Cellular  {

    public:

        LinuxCellular(const char* term, uint32_t portSpeed);
        ~LinuxCellular();

        void PowerUp() override final;

        void PowerDown() override final;

        void Restart() override final;

        uint32_t Wait(uint32_t timeout) override final;

        ssize_t Read(void *buf, size_t nbytes) override final;

        ssize_t Write(void *buf, size_t nbytes) override final;

        void InformModemHostAsleep() override final;

		void InformModemHostWakeup() override final;

        void EnterSleep() override final;

        void ExitSleep() override final;

        void SetSpeed(uint32_t portSpeed);

    private:

        static constexpr speed_t baud_bits[] = {
                0, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B921600, B1500000, B2000000, B3000000, B4000000
        };

        static const uint32_t portBaudRate = 115200;

        static const uint32_t MAX_EVENTS = 1;

        int fd = -1;

        int epoll_fd = -1;

        struct epoll_event event, events[MAX_EVENTS];

        cpp_freertos::MutexStandard serOutMutex;
    };

}


#endif //PUREPHONE_LINUX_CELLULAR_HPP
