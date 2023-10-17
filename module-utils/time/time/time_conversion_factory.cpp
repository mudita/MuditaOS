// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "time_conversion_factory.hpp"
#include "time_conversion.hpp"

namespace utils::time
{
    TimeSettingsInterface *TimestampFactory::timeSettings = nullptr;

    std::unique_ptr<Timestamp> TimestampFactory::createTimestamp(TimestampType type, time_t time)
    {
        if (timeSettings == nullptr) {
            return std::make_unique<Timestamp>(time);
        }

        switch (type) {
        case TimestampType::Timestamp:
            return std::make_unique<Timestamp>(time);
        case TimestampType::Date:
            return std::make_unique<Date>(*timeSettings, time);
        case TimestampType::DateText:
            return std::make_unique<DateText>(*timeSettings, time);
        case TimestampType::Time:
            return std::make_unique<Time>(*timeSettings, time);
        case TimestampType::DateOrTime:
            return std::make_unique<DateOrTime>(*timeSettings, time);
        case TimestampType::DateAndTime:
            return std::make_unique<DateAndTime>(*timeSettings, time);
        case TimestampType::Clock:
            return std::make_unique<Clock>(*timeSettings, time);
        }

        return std::make_unique<Timestamp>(time);
    }
} // namespace utils::time
