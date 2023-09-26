// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserEG : ImsiParser
    {
        explicit ImsiParserEG()
            : ImsiParser(
                  std::vector<std::string>{/* Orange */
                                           "60201",
                                           /* Vodafone */
                                           "60202",
                                           /* Etisalat */
                                           "60203",
                                           /* WE */
                                           "60204"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
