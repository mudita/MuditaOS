// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "AppWindow.hpp"
#include <string>

namespace app::window::name
{
    inline const std::string desktop_main_window = gui::name::window::main_window;
    inline const std::string desktop_menu        = "MenuWindow";
    inline const std::string desktop_reboot      = "Reboot";
    inline const std::string desktop_poweroff    = "PowerOffWindow";
    inline const std::string desktop_pin_lock    = "PinLockWindow";
    inline const std::string desktop_locked      = "LockedInfoWindow";
    inline const std::string desktop_update      = "Update";
}; // namespace app::window::name
