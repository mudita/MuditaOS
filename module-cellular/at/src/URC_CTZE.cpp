// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "../URC_CTZE.hpp"
#include <log/debug.hpp>

#include <module-utils/date/include/date/date.h>

using namespace at::urc;

CTZE::CTZE(const std::string &val) : Any(val)
{}

auto CTZE::what() const noexcept -> std::string
{
    return urc_name;
}

auto CTZE::isValid() const noexcept -> bool
{
    return tokens.size() == magic_enum::enum_count<Tokens>();
}

auto CTZE::getTimeInfo() const noexcept -> tm
{
    using namespace std::chrono;

    std::tm timeinfo{};
    if (isValid()) {
        std::string dateTimeStr(tokens[Tokens::Date] + "," + tokens[Tokens::Time]);
        utils::findAndReplaceAll(dateTimeStr, "\"", "");

        std::stringstream stream(dateTimeStr);
        date::sys_seconds tp;
        stream >> date::parse("%Y/%m/%d,%H:%M:%S", tp);

        auto gmtDifferenceStr = tokens[Tokens::GMTDifference];
        utils::findAndReplaceAll(gmtDifferenceStr, "\"", "");

        int gmtDifference                      = utils::getValue<int>(gmtDifferenceStr);
        constexpr auto minutesInQuartersOfHour = 15;
        constexpr auto secondsInMinutes        = 60;
        auto time = system_clock::to_time_t(tp) + gmtDifference * minutesInQuartersOfHour * secondsInMinutes;
        timeinfo  = *gmtime(&time);
    }
    return timeinfo;
}
