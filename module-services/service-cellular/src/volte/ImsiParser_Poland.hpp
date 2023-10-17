// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserPL : ImsiParser
    {
        ImsiParserPL()
            : ImsiParser(std::vector<std::string>{"26001",
                                                  "26011",
                                                  "26002",
                                                  "26010",
                                                  "26034",
                                                  "26003",
                                                  "26015",
                                                  "26016",
                                                  "26017",
                                                  "26006",
                                                  "26045",
                                                  "26098",
                                                  "26008",
                                                  "26009",
                                                  "26012",
                                                  "26013"},
                         SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
