// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <time/time_locale.hpp>

namespace utils
{
    class DateAndTimeSettings
    {
      public:
        [[nodiscard]] auto isAutomaticDateAndTimeOn() const noexcept -> bool
        {
            return automaticDateAndTimeIsOn;
        }
        [[nodiscard]] auto isAutomaticTimeZoneOn() const noexcept -> bool
        {
            return automaticTimeZoneIsOn;
        }
        [[nodiscard]] auto getTimeFormat() const noexcept -> time::Locale::TimeFormat
        {
            return timeFormat;
        }
        [[nodiscard]] auto getDateFormat() const noexcept -> time::Locale::DateFormat
        {
            return dateFormat;
        }
        [[nodiscard]] bool isTimeFormat12() const noexcept
        {
            return timeFormat == utils::time::Locale::TimeFormat::FormatTime12H;
        }
        void setAutomaticDateAndTimeOn(bool value);
        void setAutomaticTimeZoneOn(bool value);
        void setTimeFormat(time::Locale::TimeFormat format);
        void setDateFormat(time::Locale::DateFormat format);

      private:
        bool automaticDateAndTimeIsOn       = false;
        bool automaticTimeZoneIsOn          = false;
        time::Locale::TimeFormat timeFormat = time::Locale::defaultTimeFormat;
        time::Locale::DateFormat dateFormat = time::Locale::defaultDateFormat;
    };
    extern DateAndTimeSettings dateAndTimeSettings;
} // namespace utils
