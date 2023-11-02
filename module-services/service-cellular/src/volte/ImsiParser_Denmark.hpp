// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserDK : ImsiParser
    {
        ImsiParserDK()
            : ImsiParser(
                  std::vector<std::string>{/* TDC */
                                           "23801",
                                           "23810",
                                           /* Telenor */
                                           "23877",
                                           "23802",
                                           "238299",
                                           /* Telia */
                                           "23820",
                                           "23896",
                                           /* Tre (3) */
                                           "23806",
                                           /* TT-Netværket */
                                           "23866"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
