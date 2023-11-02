// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImsiParser.hpp"

namespace cellular::service
{
    struct ImsiParserGB : ImsiParser
    {
        ImsiParserGB()
            : ImsiParser(
                  std::vector<std::string>{
                      /* 3 Mobile */
                      "23420",
                      "23494",
                      /* JT */
                      "23450",
                      /* O2 */
                      "23402",
                      "23410",
                      "23411",
                      /* Orange */
                      "23433",
                      "23434",
                      /* Telet */
                      "23480",
                      /* T-Mobile */
                      "23430",
                      "23431",
                      "23432",
                      /* Vectone Mobile */
                      "23401",
                      /* Vodafone */
                      "23407",
                      "23415",
                      "23477",
                      "23489",
                      "23491",
                      "23492",
                      /* Anguilla */
                      "365010", // Weblinks Limited
                      /* Virgin Islands */
                      "348170", // Flow
                      "348770", // Digicell
                      /* Cayman Islands */
                      "34605",  // Flow
                      "346001", // Logic
                      "346140", // Digicel
                      /* Falkland Islands */
                      "750001", // Sure
                      /* Gibraltar */
                      "26601" // GibTel
                  },
                  SupportStatus::Beta)
        {}
    };
} // namespace cellular::service
