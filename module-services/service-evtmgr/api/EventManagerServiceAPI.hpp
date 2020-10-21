// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include "bsp/common.hpp" // for Board

namespace sys
{
    class Service;
} // namespace sys

namespace EventManagerServiceAPI
{
    /*
     * @brief Gets board version.
     * @return board type
     */
    bsp::Board GetBoard(sys::Service *serv);
} // namespace EventManagerServiceAPI
