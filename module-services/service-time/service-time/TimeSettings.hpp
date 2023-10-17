// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/time_conversion.hpp>
#include <service-time/api/TimeSettingsApi.hpp>

class TimeSettings : public utils::time::TimeSettingsInterface
{
  public:
    bool isTimeFormat12h() const final
    {
        return stm::api::timeFormat() == utils::time::Locale::TimeFormat::FormatTime12H;
    }
    bool isDateFormatDDMM() const final
    {
        return stm::api::dateFormat() == utils::time::Locale::DateFormat::DD_MM_YYYY;
    }
};
