
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

#include <stdint.h>
#include <stddef.h>
#include <memory>

namespace bsp {

    class Cellular {
    public:

        static std::unique_ptr<Cellular> Create(const char* term = "0");

        Cellular() {}
        virtual ~Cellular() {}


        virtual void PowerUp() = 0;

        virtual void PowerDown() = 0;

        virtual uint32_t Wait(uint32_t timeout) = 0;

        virtual uint32_t Read(void *buf, size_t nbytes) = 0;

        virtual uint32_t Write(void *buf, size_t nbytes) = 0;

    };

}


#endif //PUREPHONE_BSP_CELLULAR_HPP
