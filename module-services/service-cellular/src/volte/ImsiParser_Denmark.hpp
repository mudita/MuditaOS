// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserDK : ImsiParser
    {
        ImsiParserDK()
            : ImsiParser(std::vector<std::string>{"23801", "23802", "23806", "23820", "23866", "28801", "28802"},
                         SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
