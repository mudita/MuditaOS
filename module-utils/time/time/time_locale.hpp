// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <array>
#include <utf8/UTF8.hpp>
#include "i18n/i18n.hpp"
#include <log/log.hpp>

namespace utils
{
    namespace time
    {

        class Locale;

        extern Locale tlocale;

        class Locale
        {
          public:
            static constexpr int num_days = 7;

          private:
            static const int num_monts      = 12;
            static const int num_formatters = 9;
            // imo it would be nicer to have datetime locales in different json with thiny bit nicer and more effective
            // getters
            const std::array<std::string, num_days> daysShort = {
                "common_sun", "common_mon", "common_tue", "common_wed", "common_thu", "common_fri", "common_sat"};

            const std::array<std::string, num_days> days = {"common_sunday",
                                                            "common_monday",
                                                            "common_tuesday",
                                                            "common_wednesday",
                                                            "common_thursday",
                                                            "common_friday",
                                                            "common_saturday"};

            const std::array<std::string, num_monts> months = {"common_january",
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

            const std::array<std::string, num_formatters> time_formats{"locale_12hour_min",
                                                                       "locale_12hour_min_short",
                                                                       "locale_24hour_min",
                                                                       "locale_date_DD_MM_YYYY",
                                                                       "locale_date_MM_DD_YYYY",
                                                                       "locale_date_DD_MM",
                                                                       "locale_date_MM_DD",
                                                                       "locale_date_Day_DD_Mon",
                                                                       "locale_date_Day_Mon_DD"};

            const std::string ltoday     = "common_today";
            const std::string lyesterday = "common_yesterday";
            const std::string ltimezone  = "common_timezone";

          public:
            static constexpr int max_hour_24H_mode = 23;
            static constexpr int max_hour_12H_mode = 12;
            static constexpr int min_hour_24H_mode = 0;
            static constexpr int min_hour_12H_mode = 1;

            static constexpr int max_minutes = 59;
            static constexpr int max_years   = 2038;
            static constexpr int min_years   = 1970;

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

            enum Month
            {
                Jan = 0,
                Feb,
                Mar,
                Apr,
                May,
                Jun,
                Jul,
                Aug,
                Sept,
                Oct,
                Now,
                Dec
            };

            enum class TimeFormat
            {
                FormatTime12H,      // H:M in 12h format
                FormatTime12HShort, // H:M in 12h format, am/pm excluded
                FormatTime24H,      // H:M in 24h format
                FormatLocaleDate_DD_MM_YYYY,
                FormatLocaleDate_MM_DD_YYYY,
                FormatLocaleDate_DD_MM,
                FormatLocaleDate_MM_DD,
                FormatDate_Day_DD_Month,
                FormatDate_Day_Month_DD,
            };
            static constexpr TimeFormat defaultTimeFormat      = TimeFormat::FormatTime24H;
            static constexpr std::string_view time_format_12_H = "12 H";
            static constexpr std::string_view time_format_24_H = "24 H";

            static std::string_view get_time_format(TimeFormat timeFormat)
            {
                switch (timeFormat) {
                case TimeFormat::FormatTime12H:
                    return time_format_12_H;
                case TimeFormat::FormatTime24H:
                    return time_format_24_H;
                default:
                    return time_format_24_H;
                }
            }

            enum class DateFormat
            {
                DD_MM_YYYY = 0,
                MM_DD_YYYY,
            };
            static constexpr DateFormat defaultDateFormat = DateFormat::DD_MM_YYYY;
            static constexpr std::string_view dd_mm_yyyy  = "DD/MM/YYYY";
            static constexpr std::string_view mm_dd_yyyy  = "MM/DD/YYYY";

            static std::string_view get_date_format(DateFormat dateFormat)
            {
                switch (dateFormat) {
                case DateFormat::DD_MM_YYYY:
                    return dd_mm_yyyy;
                case DateFormat::MM_DD_YYYY:
                    return mm_dd_yyyy;
                default:
                    return dd_mm_yyyy;
                }
            }

            // this could return variant<bool, UTF8> -> on error -> false -> visit -> handle defaults
            static const UTF8 get_day(enum Day day)
            {
                if (day >= num_days) {
                    LOG_ERROR("Bad value: %d", day);
                    return "";
                }
                return translate(tlocale.days[day]);
            }

            static const UTF8 get_day(const uint32_t &day)
            {
                if (day >= num_days) {
                    return "";
                }
                return translate(tlocale.days[day]);
            }

            static const UTF8 get_short_day(const uint32_t &day)
            {
                if (day >= num_days) {
                    LOG_ERROR("Bad value");
                    return "";
                }
                return translate(tlocale.daysShort[day]);
            }

            static const UTF8 get_month(enum Month mon)
            {
                if (mon >= num_monts) {
                    LOG_ERROR("Bad value %d", mon);
                    return "";
                }
                return translate(tlocale.months[mon]);
            }

            static const UTF8 yesterday()
            {
                return translate(tlocale.lyesterday);
            }

            static const UTF8 today()
            {
                return translate(tlocale.ltoday);
            }

            static const std::string format(TimeFormat what)
            {
                return translate(tlocale.time_formats[static_cast<unsigned>(what)]);
            }

            static const UTF8 getAM()
            {
                return translate("common_AM");
            }

            static const UTF8 getPM()
            {
                return translate("common_PM");
            }
        };

    }; // namespace time
};     // namespace utils
