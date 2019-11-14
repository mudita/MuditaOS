
/*
 * @file bsp_cellular.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_BSP_CELLULAR_HPP
#define PUREPHONE_BSP_CELLULAR_HPP

#include <optional>
#include <stdint.h>
#include <stddef.h>
#include <memory>

namespace bsp {

    class Cellular {
    public:

        static std::optional<std::unique_ptr<Cellular>> Create(const char* term = "/dev/ttyUSB0", uint32_t portSpeed = 115200);

        Cellular() {}
        virtual ~Cellular() {}


        virtual void PowerUp() = 0;

        virtual void PowerDown() = 0;

        virtual void Restart() = 0;

        virtual uint32_t Wait(uint32_t timeout) = 0;

        virtual ssize_t Read(void *buf, size_t nbytes) = 0;

        virtual ssize_t Write(void *buf, size_t nbytes) = 0;

        virtual void InformModemHostAsleep() = 0;

		virtual void InformModemHostWakeup() = 0;

        virtual void EnterSleep() = 0;

        virtual void ExitSleep() = 0;


        virtual void SetSpeed(uint32_t portSpeed) = 0;

        virtual void SetSendingAllowed(bool state) = 0;
        virtual bool GetSendingAllowed() = 0;

    protected:
        bool isInitialized = false;

    };

}


#endif //PUREPHONE_BSP_CELLULAR_HPP
