// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserPL : ImsiParser
    {
        ImsiParserPL()
            : ImsiParser(
                  std::vector<std::string>{/* Polkomtel */
                                           "26001",
                                           "26004",
                                           "26011",
                                           "26015",
                                           "26016",
                                           "26017",
                                           /* T-Mobile */
                                           "26002",
                                           "26010",
                                           "26031",
                                           "26034",
                                           /* Orange */
                                           "26003",
                                           "26005",
                                           /* P4 */
                                           "26006",
                                           "26045",
                                           "26098",
                                           /* EXATEL */
                                           "26008",
                                           /* Lycamobile */
                                           "26009",
                                           /* Cyfrowy Polsat */
                                           "26012",
                                           /* Move Telecom */
                                           "26013"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
