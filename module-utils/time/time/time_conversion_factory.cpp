// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "time_conversion_factory.hpp"
#include "time_conversion.hpp"
#include "DateAndTimeSettings.hpp"

namespace utils::time
{

    namespace
    {
        class TimeSettings : public TimeSettingsInterface
        {
            bool isTimeFormat12h() const final
            {
                return utils::dateAndTimeSettings.getTimeFormat() == Locale::TimeFormat::FormatTime12H;
            }
            bool isDateFormatDDMM() const final
            {
                return utils::dateAndTimeSettings.getDateFormat() == Locale::DateFormat::DD_MM_YYYY;
            }
        };
    } // namespace

    std::unique_ptr<Timestamp> createTimestamp(TimestampType type, time_t time)
    {
        static TimeSettings timeSettings;

        switch (type) {
        case TimestampType::Timestamp:
            return std::make_unique<Timestamp>(time);
        case TimestampType::Date:
            return std::make_unique<Date>(timeSettings, time);
        case TimestampType::DateText:
            return std::make_unique<DateText>(timeSettings, time);
        case TimestampType::Time:
            return std::make_unique<Time>(timeSettings, time);
        case TimestampType::DateTime:
            return std::make_unique<DateTime>(timeSettings, time);
        case TimestampType::Clock:
            return std::make_unique<Clock>(timeSettings, time);
        }

        return std::make_unique<Timestamp>(time);
    }
} // namespace utils::time
