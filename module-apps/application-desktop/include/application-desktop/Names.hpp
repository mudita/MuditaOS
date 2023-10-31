// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace app::window::name
{
    inline constexpr auto desktop_main_window        = gui::name::window::main_window;
    inline constexpr auto desktop_menu               = "MenuWindow";
    inline constexpr auto dead_battery               = "DeadBatteryWindow";
    inline constexpr auto dead_battery_info          = "DeadBatteryInfoWindow";
    inline constexpr auto charging_battery           = "ChargingBatteryWindow";
    inline constexpr auto closing_window             = "ClosingWindow";
    inline constexpr auto desktop_mmi_pull           = "MmiPullWindow";
    inline constexpr auto desktop_mmi_push           = "MmiPushWindow";
    inline constexpr auto desktop_mmi_internal       = "MmiInternalMsgWindow";
    inline constexpr auto desktop_mmi_confirmation   = "MmiConfirmationWindow";
}; // namespace app::window::name
