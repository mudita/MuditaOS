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
                  std::vector<std::string>{/* TMobile */
                                           "26201",
                                           "26206",
                                           "26278",
                                           /* Vodafone */
                                           "26202",
                                           "26204",
                                           "26209",
                                           "26242",
                                           /* Telefonica / E-Plus */
                                           "26203",
                                           "26205",
                                           "26217",
                                           "26220",
                                           /* Telefonica / O2 */
                                           "26207",
                                           "26208",
                                           "26211",
                                           "26216",
                                           /* 1 & 1 */
                                           "26223"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
