// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserDE : ImsiParser
    {
        ImsiParserDE()
            : ImsiParser(
                  std::vector<std::string>{
                      "26201", "26206", "26202", "26204", "26209", "26203", "26205", "26208", "26223"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
