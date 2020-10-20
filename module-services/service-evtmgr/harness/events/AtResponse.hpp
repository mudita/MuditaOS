// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Events.hpp"
#include <string>
#include <vector>

namespace harness
{

    struct AtResponse : public Event<harness::Events::GSMCmd>
    {
        // encoded json event
        virtual std::string encode() final;
        std::vector<std::string> resp;
        AtResponse(const std::vector<std::string> response) : resp(response)
        {}
    };

}; // namespace harness
