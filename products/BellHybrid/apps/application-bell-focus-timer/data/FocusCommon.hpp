// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindowConstants.hpp>

namespace app::focus
{
    inline constexpr auto focusDBRecordName = "FocusTimer";

    namespace window::name
    {
        inline constexpr auto main     = gui::name::window::main_window;
        inline constexpr auto settings = "FocusTimerSettingsWindow";
        inline constexpr auto timer    = "FocusTimerSessionWindow";
    } // namespace window::name
} // namespace app::focus
