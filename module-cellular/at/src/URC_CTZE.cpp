// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "../URC_CTZE.hpp"
#include <log/debug.hpp>
#include <module-utils/time/time_conversion.hpp>

#include <module-utils/date/include/date/date.h>

using namespace at::urc;

auto CTZE::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

int CTZE::getTimeZoneOffset() const
{
    const std::string &tzOffsetToken = tokens[static_cast<uint32_t>(Tokens::GMTDifference)];

    int offsetInQuartersOfHour = 0;
    bool failed                = !utils::toNumeric(tzOffsetToken, offsetInQuartersOfHour);

    if (offsetInQuartersOfHour != std::clamp(offsetInQuartersOfHour, minTimezoneOffset, maxTimezoneOffset)) {
        offsetInQuartersOfHour = 0;
        failed                 = true;
    }

    if (failed) {
        LOG_ERROR("Failed to parse CTZE time zone offset: %s", tzOffsetToken.c_str());
    }

    int offsetInSeconds = offsetInQuartersOfHour * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute;

    return offsetInSeconds;
}

std::string CTZE::getTimeZoneString() const
{
    std::string timeZoneStr = tokens[static_cast<uint32_t>(Tokens::GMTDifference)] + "," +
                              tokens[static_cast<uint32_t>(Tokens::DaylightSavingsAdjustment)];
    timeZoneStr.erase(remove_if(timeZoneStr.begin(), timeZoneStr.end(), isspace), timeZoneStr.end());
    return timeZoneStr;
}

const struct tm CTZE::getGMTTime(void) const
{
    struct tm timeinfo = {};
    std::stringstream stream(tokens[static_cast<uint32_t>(Tokens::Date)] + "," +
                             tokens[static_cast<uint32_t>(Tokens::Time)]);
    stream >> std::get_time(&timeinfo, "%Y/%m/%d,%H:%M:%S");
    if (stream.fail()) {
        LOG_ERROR("Failed to parse CTZE time");
    }
    return timeinfo;
}

auto CTZE::getTimeInfo() const noexcept -> tm
{
    using namespace std::chrono;

    std::tm timeinfo{};
    if (isValid()) {
        std::string dateTimeStr(tokens[Tokens::Date] + "," + tokens[Tokens::Time]);

        std::stringstream stream(dateTimeStr);
        date::sys_seconds tp;
        stream >> date::parse("%Y/%m/%d,%H:%M:%S", tp);

        auto gmtDifferenceStr = tokens[Tokens::GMTDifference];

        int gmtDifference = utils::getValue<int>(gmtDifferenceStr);
        auto time         = system_clock::to_time_t(tp) +
                    gmtDifference * utils::time::minutesInQuarterOfHour * utils::time::secondsInMinute;
        timeinfo = *gmtime(&time);
    }
    return timeinfo;
}
