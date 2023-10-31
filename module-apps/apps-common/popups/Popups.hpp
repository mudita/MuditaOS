// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace gui
{
    namespace popup
    {
        enum class ID
        {
            Volume,
            PhoneModes,
            Brightness,
            Tethering,
            TetheringPhoneModeChangeProhibited,
            BluetoothAuthenticate,
            BluetoothInfo,
            PhoneLock,
            PhoneLockInput,
            PhoneLockInfo,
            PhoneLockChangeInfo,
            SimLock,
            SimInfo,
            SimNotReady,
            SimSwitching,
            AlarmActivated,
            AlarmDeactivated,
            Alarm,
            PowerOff,
            Reboot,
            BedtimeNotification,
            ChargingNotification,
            ChargingDoneNotification,
            AppTestPopup,
            Invalid,
        };

        namespace window
        {
            inline constexpr auto volume_window                     = "VolumePopup";
            inline constexpr auto phone_modes_window                = "PhoneModesPopup";
            inline constexpr auto brightness_window                 = "BrightnessPopup";
            inline constexpr auto tethering_confirmation_window     = "TetheringConfirmationPopup";
            inline constexpr auto tethering_phonemode_change_window = "TetheringPhoneModeChangeProhibitedPopup";
            inline constexpr auto tethering_off_window              = "TetheringOffPopup";
            inline constexpr auto tethering_menu_window             = "TetheringMenuPopup";
            inline constexpr auto bluetooth_authenticate            = "BluetoothAuthenticatePopup";
            inline constexpr auto bluetooth_info                    = "BluetoothInfoPopup";
            inline constexpr auto phone_lock_window                 = "PhoneLockPopup";
            inline constexpr auto phone_lock_info_window            = "PhoneLockInfoPopup";
            inline constexpr auto phone_lock_input_window           = "PhoneLockInputPopup";
            inline constexpr auto phone_lock_change_info_window     = "PhoneLockChangeInfoPopup";
            inline constexpr auto power_off_window                  = "PowerOffPopup";
            inline constexpr auto sim_unlock_window                 = "SimUnlockPopup";
            inline constexpr auto sim_info_window                   = "SimInfoPopup";
            inline constexpr auto sim_not_ready_window              = "SimNotReadyPopup";
            inline constexpr auto sim_switching_window              = "SimSwitchingPopup";
            inline constexpr auto alarm_activated_window            = "AlarmActivatedPopup";
            inline constexpr auto alarm_deactivated_window          = "AlarmDeactivatedPopup";
            inline constexpr auto alarm_window                      = "AlarmPopup";
            inline constexpr auto reboot_window                     = "RebootPopup";
            inline constexpr auto bedtime_notification_window       = "BedtimeNotificationPopup";
            inline constexpr auto charging_notification_window      = "ChargingNotificationPopup";
            inline constexpr auto charging_done_notification_window = "ChargingDoneNotificationPopup";
            inline constexpr auto test_popup                        = "test_popup";
        } // namespace window

        std::string resolveWindowName(ID id);
    } // namespace popup
} // namespace gui
