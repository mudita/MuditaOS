// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_LINUX_CELLULAR_HPP
#define PUREPHONE_LINUX_CELLULAR_HPP

#include "bsp/cellular/bsp_cellular.hpp"

#include <cstring>
#include <sys/epoll.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <mutex.hpp>

namespace bsp
{
    class LinuxCellular : public Cellular
    {
      private:
        bool pv_SendingAllowed = true;

      public:
        LinuxCellular(const char *term, uint32_t portSpeed);
        ~LinuxCellular();

        void powerUp() override final;
        void powerDown() override final;
        void restart() override final;

        uint32_t wait(std::chrono::milliseconds timeoutMs) override final;
        ssize_t read(void *buf, size_t nbytes, std::chrono::milliseconds timeoutMs) override final;
        ssize_t write(void *buf, size_t nbytes) override final;

        void informModemHostAsleep() override final;
        void informModemHostWakeup() override final;

        void enterSleep() override final;
        void exitSleep() override final;
        void setSpeed(uint32_t portSpeed) override final;

        void setSendingAllowed(bool state) override final
        {
            pv_SendingAllowed = state;
        }
        bool getSendingAllowed() const noexcept override final
        {
            return pv_SendingAllowed;
        }

        void selectAntenna(bsp::cellular::antenna antenna) override final;
        bsp::cellular::antenna getAntenna() override final;

      private:
        static constexpr speed_t baud_bits[] = {0,
                                                B9600,
                                                B19200,
                                                B38400,
                                                B57600,
                                                B115200,
                                                B230400,
                                                B460800,
                                                B921600,
                                                B1500000,
                                                B2000000,
                                                B3000000,
                                                B4000000};

        static const uint32_t MAX_EVENTS = 1;

        int fd = -1;

        int epoll_fd = -1;

        struct epoll_event event, events[MAX_EVENTS];

        cpp_freertos::MutexStandard serOutMutex;
    };

} // namespace bsp

#endif // PUREPHONE_LINUX_CELLULAR_HPP
