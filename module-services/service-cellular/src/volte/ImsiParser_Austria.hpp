// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    ;

    struct ImsiParserAT : ImsiParser
    {
        explicit ImsiParserAT()
            : ImsiParser(std::vector<std::string>{"23201", "23203", "23213", "23205", "23210", "23207", "23208"})
        {}
    };
} // namespace cellular::service
