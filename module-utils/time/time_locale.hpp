// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <array>
#include <utf8/UTF8.hpp>
#include "i18n/i18n.hpp"
#include <log/log.hpp>
#include <module-utils/date/include/date/date.h>

namespace utils
{
    namespace time
    {

        class Locale;

        extern Locale tlocale;

        class Locale
        {
            static const int num_days       = 7;
            static const int num_formatters = 5;
            // imo it would be nicer to have datetime locales in different json with thiny bit nicer and more effective
            // getters
            const std::array<std::string, num_days> daysShort = {
                "common_mo", "common_tu", "common_we", "common_th", "common_fr", "common_sa", "common_su"};

            const std::array<std::string, num_days> days = {"common_sunday",
                                                            "common_monday",
                                                            "common_tuesday",
                                                            "common_wednesday",
                                                            "common_thursday",
                                                            "common_friday",
                                                            "common_saturday"};

            const std::array<std::string, date::months(date::years{1}).count()> months = {"common_january",
                                                               "common_february",
                                                               "common_march",
                                                               "common_april",
                                                               "common_may",
                                                               "common_june",
                                                               "common_july",
                                                               "common_august",
                                                               "common_september",
                                                               "common_october",
                                                               "common_november",
                                                               "common_december"};

            const std::array<std::string, num_formatters> time_formats{
                "locale_12hour_min",
                "locale_12hour_min_short",
                "locale_24hour_min",
                "locale_date_full",
                "locale_date_short",
            };

            const std::string ltoday     = "common_today";
            const std::string lyesterday = "common_yesterday";
            const std::string ltimezone  = "common_timezone";

          public:
            enum Day
            {
                Sun = 0,
                Mon,
                Tue,
                Wed,
                Thu,
                Fri,
                Sat,
            };

            enum TimeFormat
            {
                FormatTime12H = 0,     // H:M in 12h format
                FormatTime12HShort,    // H:M in 12h format, am/pm excluded
                FormatTime24H,         // H:M in 24h format
                FormatLocaleDateFull,  // format locale specified format
                FormatLocaleDateShort, // format locale specified format
            };

            // this could return variant<bool, UTF8> -> on error -> false -> visit -> handle defaults
            static const UTF8 get_day(enum Day day)
            {
                if (day >= num_days) {
                    LOG_ERROR("Bad value: %d", day);
                    return "";
                }
                else {
                    return localize.get(tlocale.days[day]);
                }
            }

            static const UTF8 get_short_day(const uint32_t &day)
            {
                if (day >= num_days) {
                    LOG_ERROR("Bad value");
                    return "";
                }
                else {
                    return localize.get(tlocale.daysShort[day]);
                }
            }

            static const UTF8 get_month(date::month mon)
            {
                auto monthNum = static_cast<uint32_t>(mon);
                if (monthNum > date::months(date::years{1}).count()) {
                    LOG_ERROR("Bad value %d", monthNum);
                    return "";
                }
                else {
                    return localize.get(tlocale.months[monthNum-1]);
                }
            }

            static const UTF8 yesterday()
            {
                return localize.get(tlocale.lyesterday);
            }

            static const UTF8 today()
            {
                return localize.get(tlocale.ltoday);
            }

            static const std::string format(enum TimeFormat what)
            {
                return localize.get(tlocale.time_formats[what]);
            }

            static const UTF8 getAM()
            {
                return localize.get("common_AM");
            }

            static const UTF8 getPM()
            {
                return localize.get("common_PM");
            }
        };

    }; // namespace time
};     // namespace utils
