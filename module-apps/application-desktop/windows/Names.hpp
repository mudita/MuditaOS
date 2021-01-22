// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "AppWindow.hpp"

namespace app::window::name
{
    inline constexpr auto desktop_main_window = gui::name::window::main_window;
    inline constexpr auto desktop_menu         = "MenuWindow";
    inline constexpr auto desktop_reboot       = "Reboot";
    inline constexpr auto desktop_poweroff     = "PowerOffWindow";
    inline constexpr auto dead_battery         = "DeadBatteryWindow";
    inline constexpr auto desktop_pin_lock     = "PinLockWindow";
    inline constexpr auto desktop_locked       = "LockedInfoWindow";
    inline constexpr auto desktop_update       = "Update";
    inline constexpr auto desktop_mmi_pull     = "MmiPullWindow";
    inline constexpr auto desktop_mmi_push     = "MmiPushWindow";
    inline constexpr auto desktop_mmi_internal = "MmiInternalMsgWindow";
}; // namespace app::window::name
