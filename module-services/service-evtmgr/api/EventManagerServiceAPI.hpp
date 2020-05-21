#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include "bsp/common.hpp"

namespace EventManagerServiceAPI
{
    /*
     * @brief Gets board version.
     * @return board type
     */
    bsp::Board GetBoard(sys::Service *serv);
} // namespace EventManagerServiceAPI
