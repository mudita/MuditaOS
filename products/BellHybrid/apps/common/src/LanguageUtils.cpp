// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>

namespace
{
    std::string transform(const std::uint32_t val,
                          const std::string &minuteLower,
                          const std::string &minutesLower,
                          const std::string &minutesLowerGenitive)
    {
        if (val == 1) {
            return minuteLower;
        }
        auto core = val % 100;
        if (utils::getDisplayLanguage() == "Polski") {
            if (core < 10 || core > 20) {
                if ((core % 10) == 2 || (core % 10) == 3 || (core % 10) == 4) {
                    return minutesLower;
                }
            }
            return minutesLowerGenitive;
        }
        return minutesLower;
    }
} // namespace

namespace utils::language
{
    auto getCorrectMinutesNumeralForm(const std::uint32_t val) -> std::string
    {
        return transform(val,
                         utils::translate("common_minute_lower"),
                         utils::translate("common_minutes_lower"),
                         utils::translate("common_minutes_lower_genitive"));
    }

    auto getCorrectSecondsNumeralForm(const std::uint32_t val) -> std::string
    {
        return transform(val,
                         utils::translate("common_second_lower"),
                         utils::translate("common_seconds_lower"),
                         utils::translate("common_seconds_lower_genitive"));
    }
} // namespace utils::language
