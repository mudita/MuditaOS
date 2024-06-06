// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindowConstants.hpp>
#include <Paths.hpp>
#include <filesystem>
#include <string>

namespace app::focus
{
    namespace window::name
    {
        inline constexpr auto main                 = gui::name::window::main_window;
        inline constexpr auto settings             = "FocusTimerSettingsWindow";
        inline constexpr auto timer                = "FocusTimerSessionWindow";
        inline constexpr auto focusTimerLowBattery = "FocusTimerLowBatteryWindow";
    } // namespace window::name

    inline std::filesystem::path getFocusTimerAudioPath()
    {
        return paths::audio::proprietary() / paths::audio::focusTimer() / "FocusTimer_Gong.mp3";
    }

    namespace models::settings
    {
        inline constexpr auto focusTimeMin     = 5U;
        inline constexpr auto focusTimeMax     = 60U;
        inline constexpr auto focusTimeStep    = 5U;
        inline constexpr auto focusTimeDefault = 25U;
        inline constexpr auto focusTimeName    = "focus_time";

        inline constexpr auto focusRepeatsMin     = 1U;
        inline constexpr auto focusRepeatsMax     = 19U;
        inline constexpr auto focusRepeatsStep    = 1U;
        inline constexpr auto focusRepeatsDefault = 10U;
        inline constexpr auto focusRepeatsName    = "focus_repeats";

        inline constexpr auto shortBreakTimeMin     = 0U;
        inline constexpr auto shortBreakTimeMax     = 15U;
        inline constexpr auto shortBreakTimeStep    = 1U;
        inline constexpr auto shortBreakTimeDefault = 5U;
        inline constexpr auto shortBreakTimeName    = "short_break";

        inline constexpr auto longBreakTimeMin     = 0U;
        inline constexpr auto longBreakTimeMax     = 30U;
        inline constexpr auto longBreakTimeStep    = 1U;
        inline constexpr auto longBreakTimeDefault = 15U;
        inline constexpr auto longBreakTimeName    = "long_break_time";

        inline constexpr auto longBreakOccurrenceMin     = 1U;
        inline constexpr auto longBreakOccurrenceMax     = 19U;
        inline constexpr auto longBreakOccurrenceStep    = 1U;
        inline constexpr auto longBreakOccurrenceDefault = 4U;
        inline constexpr auto longBreakOccurrenceName    = "long_break_occurrence";
    } // namespace models::settings
} // namespace app::focus
