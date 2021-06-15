// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

namespace utils::time
{
    /** @brief get all available TimeZones to display
     *
     *  @return vector of string with TimeZones names
     */
    [[nodiscard]] auto getAvailableTimeZonesToDisplay() -> std::vector<std::string>;

    /** @brief get offset of TimeZone
     *
     *  @param zoneName - name of TimeZone
     *  @return offset hours and minutes with a sign in the interval of 15 minutes [6 => 1h 30 min]
     */
    [[nodiscard]] auto getTimeZoneOffset(const std::string &zoneName) -> std::int8_t;

    /** @brief get offset and DST rules of TimeZone
     *
     *  @param zoneName - name of TimeZone
     *  @return string with rules to set TimeZone
     */
    [[nodiscard]] auto getTimeZoneRules(const std::string &zoneName) -> std::string;

} // namespace utils::time
