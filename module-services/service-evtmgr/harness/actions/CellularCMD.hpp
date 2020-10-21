// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Service/Service.hpp>

#include <string> // for string

namespace sys
{
    class Service;
} // namespace sys

namespace harness
{
    namespace action
    {
        bool gsm_send(sys::Service *serv, const std::string &cmd);
    }
} // namespace harness
