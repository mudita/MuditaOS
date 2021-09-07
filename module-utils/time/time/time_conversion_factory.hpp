// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "time_conversion.hpp"
#include <memory>

namespace utils::time
{
    /// Most types are system locale dependant
    enum class TimestampType
    {
        Timestamp, /// no default formatting, no system locale dependencies
        Clock,     /// actual time
        Time,      /// past time
        DateTime,  /// system date time formatting
        Date,      /// date
        DateText,  /// date in textual represnatation
    };
    class TimestampFactory
    {
      private:
        static TimeSettingsInterface *timeSettings;

      public:
        TimestampFactory() = default;
        void init(TimeSettingsInterface *settings)
        {
            timeSettings = settings;
        }
        std::unique_ptr<Timestamp> createTimestamp(TimestampType type, time_t time = 0);
    };
} // namespace utils::time
