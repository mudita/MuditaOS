#pragma once

#include <array>
#include <utf8/UTF8.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>

namespace utils
{
    namespace time
    {

        class Locale;

        extern Locale tlocale;

        class Locale
        {
            static const int num_days       = 7;
            static const int num_monts      = 12;
            static const int num_formatters = 4;
            // imo it would be nicer to have datetime locales in different json with thiny bit nicer and more effective
            // getters
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

            const std::array<std::string, num_formatters> time_formats{
                "locale_12hour_min",
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

            enum TimeFormat
            {
                FormatTime12H = 0,     // H:M in 12h format
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

            static const UTF8 get_month(enum Month mon)
            {
                if (mon >= num_monts) {
                    LOG_ERROR("Bad value %d", mon);
                    return "";
                }
                else {
                    return localize.get(tlocale.months[mon]);
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
                if (what >= num_formatters) {
                    LOG_ERROR("Bad value: %d", what);
                    return "⌚";
                }
                return localize.get(tlocale.time_formats[what]);
            }
        };

    }; // namespace time
};     // namespace utils
