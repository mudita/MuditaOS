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
                  std::vector<std::string>{/* KPN Mobile */
                                           "20412",
                                           "20408",
                                           "20410",
                                           "20469",
                                           /* Move/Teleena */
                                           "20407",
                                           /* T-Mobile */
                                           "20402",
                                           "20416",
                                           "20420",
                                           /* Truphone */
                                           "20433",
                                           /* Vectone Mobile */
                                           "204299",
                                           /* Vodafone */
                                           "20404",
                                           /* Ziggo */
                                           "20415",
                                           "20418"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
