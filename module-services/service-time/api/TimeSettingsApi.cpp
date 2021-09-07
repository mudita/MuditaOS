// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSettingsApi.hpp"
#include <internal/StaticData.hpp>

#include <time/time_locale.hpp>

namespace stm::api
{
    bool isAutomaticDateAndTime()
    {
        return stm::internal::StaticData::get().getAutomaticDateAndTime();
    }

    utils::time::Locale::DateFormat dateFormat()
    {
        return stm::internal::StaticData::get().getDateFormat();
    }

    utils::time::Locale::TimeFormat timeFormat()
    {
        return stm::internal::StaticData::get().getTimeFormat();
    }
    bool isTimeFormat12h()
    {
        return stm::internal::StaticData::get().getTimeFormat() == utils::time::Locale::TimeFormat::FormatTime12H;
    }
    const std::string getCurrentTimezoneName()
    {
        return stm::internal::StaticData::get().getCurrentTimezoneName();
    }
    const std::string getCurrentTimezoneRules()
    {
        return stm::internal::StaticData::get().getCurrentTimezoneRules();
    }
} // namespace stm::api
