// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Utils.hpp"
#include "dateCommon.hpp"

namespace utils::time
{
    enum class Version
    {
        abbrev,
        normal
    };

    class TimeRangeParser
    {

      private:
        std::string AMPMtoString(bool isAm);

      public:
        std::string getCalendarTimeString(TimePoint startDate,
                                          TimePoint endDate,
                                          Version version = Version::normal,
                                          bool isMode24H  = false);
    };
} // namespace utils::time
