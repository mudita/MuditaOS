// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bell::settings
{
    namespace Temperature
    {
        constexpr inline auto unit = "temperature_unit";
    } // namespace Temperature
    namespace Snooze
    {
        constexpr inline auto active   = "snooze_active";
        constexpr inline auto length   = "snooze_length";
        constexpr inline auto interval = "snooze_interval";
        constexpr inline auto tone     = "snooze_tone";
    } // namespace Snooze
    namespace PrewakeUp
    {
        constexpr inline auto duration      = "prewake_up_duration";
        constexpr inline auto tone          = "prewake_up_tone";
        constexpr inline auto lightDuration = "prewake_up_light_duration";
    } // namespace PrewakeUp
    namespace Alarm
    {
        constexpr inline auto tone        = "alarm_tone";
        constexpr inline auto lightActive = "alarm_light_active";
        constexpr inline auto duration    = "alarm_duration";
    } // namespace Alarm
    namespace Bedtime
    {
        constexpr inline auto active   = "bedtime_active";
        constexpr inline auto time     = "bedtime_time";
        constexpr inline auto tone     = "bedtime_tone";
        constexpr inline auto duration = "bedtime_duration";
    } // namespace Bedtime
    namespace Layout
    {
        constexpr inline auto layout = "layout";
    } // namespace Layout
};    // namespace bell::settings
