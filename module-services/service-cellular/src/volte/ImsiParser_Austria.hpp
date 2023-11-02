// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserAT : ImsiParser
    {
        explicit ImsiParserAT()
            : ImsiParser(
                  std::vector<std::string>{/* A1 Telekom */
                                           "23211",
                                           "23202",
                                           "23209",
                                           "23201",
                                           "23212",
                                           /* T-Mobile/Magenta */
                                           "23204",
                                           "23203",
                                           "23223",
                                           "23213",
                                           /* Drei (3) */
                                           "23219",
                                           "23205",
                                           "23216",
                                           "23210",
                                           "23214",
                                           /* Ventocom */
                                           "23207",
                                           /* Lycamobile AT */
                                           "23208"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
