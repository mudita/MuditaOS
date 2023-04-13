// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserCA : ImsiParser
    {
        explicit ImsiParserCA()
            : ImsiParser(std::vector<std::string>{
                  "302220", "302270", "302300", "302310", "302490", "302510", "302500", "302610", "302720", "302780"})
        {}
    };
} // namespace cellular::service
