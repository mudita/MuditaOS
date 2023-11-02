// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserCA : ImsiParser
    {
        ImsiParserCA()
            : ImsiParser(
                  std::vector<std::string>{/* Bell Mobility */
                                           "302630",
                                           "302660",
                                           "30264",
                                           "302690",
                                           "30269",
                                           "302610",
                                           "302640",
                                           "30261",
                                           "30266",
                                           "30263",
                                           /* ECOTEL */
                                           "302300",
                                           "302310",
                                           /* Eastlink */
                                           "302270",
                                           /* Freedom Mobile */
                                           "30249",
                                           "302491",
                                           "302490",
                                           /* Rogers */
                                           "30232",
                                           "30282",
                                           "302320",
                                           "302820",
                                           "30272",
                                           "302720",
                                           "302721",
                                           /* SaskTel Mobility */
                                           "30275",
                                           "30278",
                                           "302681",
                                           "302780",
                                           "302781",
                                           "302750",
                                           "30268",
                                           "302680",
                                           /* Telus Mobility */
                                           "302220",
                                           "302760",
                                           "30222",
                                           "302860",
                                           "30276",
                                           "302360",
                                           "302221",
                                           "30286",
                                           "30236",
                                           /* Videotron */
                                           "30250",
                                           "302510",
                                           "302520",
                                           "302500",
                                           "30251"},
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
