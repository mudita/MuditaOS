// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * time_date_validation.cpp
 *
 *  Created on: 31 sty 2020
 *      Author: kuba
 */
#include "time_date_validation.hpp"
#include <log/log.hpp>
namespace utils
{
    namespace time
    {
        bool validateDate(uint32_t day, uint32_t month, uint32_t year)
        {
            if (month < 1 || month > 12) {
                return false;
            }
            if (day < 1 || day > 31) {
                return false;
            }
            if ((day == 31) && (month == 2 || month == 4 || month == 6 || month == 9 || month == 11)) {
                return false;
            }
            if (day == 30 && month == 2) {
                return false;
            }
            if (day == 29 && month == 2 && year % 4 != 0) {
                return false;
            }
            if ((month == 2) && (day == 29) && (year % 400 == 0)) {
                return true;
            }
            if ((month == 2) && (day == 29) && (year % 100 == 0)) {
                return false;
            }
            if ((month == 2) && (day == 29) && (year % 4 == 0)) {
                return true;
            }
            if (year > 2050) {
                return false;
            }
            return true;
        }

        bool validateDate(std::string day, std::string month, std::string year)
        {
            uint32_t d = 0, m = 0, y = 0;
            try {
                d = std::stoi(day);
                m = std::stoi(month);
                y = std::stoi(year);
            }
            catch (std::exception &e) {
                return false;
            }
            return validateDate(d, m, y);
        }

        bool validateTime(std::string hour, std::string minute, bool mode12h)
        {
            uint32_t m = 0, h = 0;
            try {
                h = std::stoi(hour);
                m = std::stoi(minute);
            }
            catch (std::exception &e) {
                LOG_INFO("validateTime exception");
                return false;
            }
            return validateTime(h, m, mode12h);
        }
        bool validateTime(uint32_t hour, uint32_t minute, bool mode12h)
        {

            unsigned int hourMax = 0;
            if (mode12h) {
                hourMax = 12;
            }
            else {
                hourMax = 23;
            }

            if (hour > hourMax) {
                return false;
            }
            if (minute > 59) {
                return false;
            }
            return true;
        }
    } // namespace time
} // namespace utils
