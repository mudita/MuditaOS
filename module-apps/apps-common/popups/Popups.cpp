// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Popups.hpp"

namespace gui::popup
{
    std::string resolveWindowName(ID id)
    {
        switch (id) {
        case ID::Volume:
            return gui::popup::window::volume_window;
        case ID::PhoneModes:
            return gui::popup::window::phone_modes_window;
        case ID::Brightness:
            return gui::popup::window::brightness_window;
        case ID::Tethering:
            return gui::popup::window::tethering_confirmation_window;
        case ID::TetheringPhoneModeChangeProhibited:
            return gui::popup::window::tethering_phonemode_change_window;
        case ID::PhoneLock:
            return gui::popup::window::phone_lock_window;
        case ID::PhoneLockInput:
            return gui::popup::window::phone_lock_input_window;
        case ID::PhoneLockInfo:
            return gui::popup::window::phone_lock_info_window;
        case ID::PhoneLockChangeInfo:
            return gui::popup::window::phone_lock_change_info_window;
        case ID::SimLock:
            return gui::popup::window::sim_unlock_window;
        case ID::SimInfo:
            return gui::popup::window::sim_info_window;
        case ID::SimNotReady:
            return gui::popup::window::sim_not_ready_window;
        case ID::AlarmActivated:
            return gui::popup::window::alarm_activated_window;
        }

        return {};
    }
} // namespace gui::popup
