// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace utils::time
{
    constexpr uint8_t timeZoneNameOffset{10};
    constexpr auto defaultTimeZoneName{"London"};

    /** @brief get all available TimeZones with offsets
     *
     *  @return vector of string with TimeZones names and offsets
     */
    [[nodiscard]] auto getAvailableTimeZonesWithOffset() -> std::vector<std::string>;

    /** @brief get offset of TimeZone include DST rules
     *
     *  @param zoneName - name of TimeZone
     *  @param time - date and time for the offset
     *  @return offset in seconds with a sign
     */
    [[nodiscard]] auto getTimeZoneOffset(const std::string &zoneName, const time_t &time) -> std::chrono::seconds;

    /** @brief get offset and DST rules of TimeZone
     *
     *  @param zoneName - name of TimeZone
     *  @return string with rules to set TimeZone
     */
    [[nodiscard]] auto getTimeZoneRules(const std::string &zoneName) -> std::string;

} // namespace utils::time
