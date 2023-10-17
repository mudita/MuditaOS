// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * time_date_validation.hpp
 *
 *  Created on: 31 sty 2020
 *      Author: kuba
 */

#pragma once
#include <string>

namespace utils
{
    namespace time
    {
        bool validateDate(uint32_t day, uint32_t month, uint32_t year);
        bool validateDate(std::string day, std::string month, std::string year);

        bool validateTime(std::string hour, std::string minute, bool mode12h = false);
        bool validateTime(uint32_t hour, uint32_t minute, bool mode12h = false);
    } // namespace time
} // namespace utils
