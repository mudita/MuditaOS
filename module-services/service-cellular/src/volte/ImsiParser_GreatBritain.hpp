// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserGB : ImsiParser
    {
        ImsiParserGB()
            : ImsiParser(std::vector<std::string>{"23401",
                                                  "23410",
                                                  "23411",
                                                  "23403",
                                                  "23415",
                                                  "23420",
                                                  "23430",
                                                  "23433",
                                                  "23434",
                                                  "23450",
                                                  "23480",
                                                  "365010",
                                                  "348170",
                                                  "348770",
                                                  "34605",
                                                  "346001",
                                                  "346140",
                                                  "750001",
                                                  "26601"},
                         SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
