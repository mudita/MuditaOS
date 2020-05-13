#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include "bsp/common.hpp"

namespace EventServiceAPI
{
    /*
     * @brief Gets platform version.
     * @return true on T4 platform, false on T3 platform
     */
    bsp::Board GetBoard(sys::Service *serv);
} // namespace EventServiceAPI
