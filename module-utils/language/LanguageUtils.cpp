// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LanguageUtils.hpp"
#include <i18n/i18n.hpp>

namespace
{
    constexpr auto polishLanguageName = "Polski";

    std::string transformNumeral(std::uint32_t val,
                                 const std::string &minuteLower,
                                 const std::string &minutesLower,
                                 const std::string &minutesLowerGenitive)
    {
        if (val == 1) {
            return minuteLower;
        }
        auto core = val % 100;
        if (utils::getDisplayLanguage() == polishLanguageName) {
            if (core < 10 || core > 20) {
                if ((core % 10) == 2 || (core % 10) == 3 || (core % 10) == 4) {
                    return minutesLower;
                }
            }
            return minutesLowerGenitive;
        }
        return minutesLower;
    }

    std::string transformAccusative(std::uint32_t val,
                                    const std::string &minuteLower,
                                    const std::string &minuteAccusative,
                                    const std::string &minutesLower,
                                    const std::string &minutesLowerGenitive)
    {
        if ((val == 1) && (utils::getDisplayLanguage() == polishLanguageName)) {
            return minuteAccusative;
        }
        return transformNumeral(val, minuteLower, minutesLower, minutesLowerGenitive);
    }

    std::string transformMultiplicity(std::uint32_t val, const std::string &once, const std::string &many)
    {
        return (val == 1) ? once : many;
    }
} // namespace

namespace utils::language
{
    auto getCorrectMinutesNumeralForm(std::uint32_t val) -> std::string
    {
        return transformNumeral(val,
                                utils::translate("common_minute_lower"),
                                utils::translate("common_minutes_lower"),
                                utils::translate("common_minutes_lower_genitive"));
    }

    auto getCorrectSecondsNumeralForm(std::uint32_t val) -> std::string
    {
        return transformNumeral(val,
                                utils::translate("common_second_lower"),
                                utils::translate("common_seconds_lower"),
                                utils::translate("common_seconds_lower_genitive"));
    }

    auto getCorrectMinutesAccusativeForm(std::uint32_t val) -> std::string
    {
        return transformAccusative(val,
                                   utils::translate("common_minute_lower"),
                                   utils::translate("common_minute_lower_accusative"),
                                   utils::translate("common_minutes_lower"),
                                   utils::translate("common_minutes_lower_genitive"));
    }

    auto getCorrectMultiplicityForm(std::uint32_t val) -> std::string
    {
        return transformMultiplicity(
            val, utils::translate("common_multiplicity_once"), utils::translate("common_multiplicity_many"));
    }
} // namespace utils::language
