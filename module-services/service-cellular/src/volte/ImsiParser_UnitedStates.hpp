// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"
#include <vector>

namespace cellular::service
{
    struct ImsiParserUS : ImsiParser
    {
        explicit ImsiParserUS()
            : ImsiParser(std::vector<std::string>{"310053",
                                                  "310120",
                                                  "310260",
                                                  "310530",
                                                  "310770",
                                                  "311490",
                                                  "311660",
                                                  "311880",
                                                  "311882",
                                                  "312190",
                                                  "312250",
                                                  "312530"})
        {}
    };
} // namespace cellular::service
