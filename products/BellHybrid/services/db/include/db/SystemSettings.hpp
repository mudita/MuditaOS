// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace bell::settings
{
    namespace Temperature
    {
        inline constexpr auto unit = "temperature_unit";
    } // namespace Temperature

    namespace Snooze
    {
        inline constexpr auto tonePath         = "snooze_tone_path";
        inline constexpr auto toneFallbackPath = "snooze_tone_path_default";
        inline constexpr auto active           = "snooze_active";
        inline constexpr auto length           = "snooze_length";
        inline constexpr auto interval         = "snooze_interval";
    } // namespace Snooze

    namespace PrewakeUp
    {
        inline constexpr auto tonePath         = "prewake_up_tone_path";
        inline constexpr auto toneFallbackPath = "prewake_up_tone_path_default";
        inline constexpr auto duration         = "prewake_up_duration";
        inline constexpr auto lightDuration    = "prewake_up_light_duration";
        inline constexpr auto brightness       = "prewake_up_brightness";
    } // namespace PrewakeUp

    namespace Alarm
    {
        inline constexpr auto tonePath         = "alarm_tone_path";
        inline constexpr auto toneFallbackPath = "alarm_tone_path_default";
        inline constexpr auto fadeActive       = "alarm_fade_active";
        inline constexpr auto lightActive      = "alarm_light_active";
        inline constexpr auto duration         = "alarm_duration";
        inline constexpr auto brightness       = "alarm_brightness";
    } // namespace Alarm

    namespace Bedtime
    {
        inline constexpr auto tonePath         = "bedtime_tone_path";
        inline constexpr auto toneFallbackPath = "bedtime_tone_path_default";
        inline constexpr auto active           = "bedtime_active";
        inline constexpr auto time             = "bedtime_time";
        inline constexpr auto duration         = "bedtime_duration";
    } // namespace Bedtime

    namespace Relaxation
    {
        inline constexpr auto fadeActive = "relaxation_fade_active";
    }

    namespace Layout
    {
        inline constexpr auto layout = "layout";
    } // namespace Layout
} // namespace bell::settings
