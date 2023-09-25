// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserNL : ImsiParser
    {
        ImsiParserNL()
            : ImsiParser(
                  std::vector<std::string>{
                      "20402", "20404", "20406", "20407", "20408", "20410", "20416", "20420", "20433", "20418"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
