// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>

namespace utils::language
{
    auto getCorrectMinutesNumeralForm(int val) -> std::string
    {
        if (val == 1) {
            return utils::translate("common_minute_lower");
        }
        if (utils::getDisplayLanguage() == "Polski") {
            if (val < 10 || val > 20) {
                if ((val % 10) == 2 || (val % 10) == 3 || (val % 10) == 4) {
                    return utils::translate("common_minutes_lower");
                }
            }
            return utils::translate("common_minutes_lower_genitive");
        }
        return utils::translate("common_minutes_lower");
    }
} // namespace utils::language
