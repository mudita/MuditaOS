// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace settings
{
    namespace SystemProperties
    {
        inline constexpr auto activeSim                = "gs_active_sim";
        inline constexpr auto lockPassHash             = "gs_lock_pass_hash";
        inline constexpr auto lockScreenPasscodeIsOn   = "gs_lock_screen_passcode_is_on";
        inline constexpr auto autoLockTimeInSec        = "gs_auto_lock_time";
        inline constexpr auto unlockLockTime           = "gs_unlock_lock_time";
        inline constexpr auto unlockAttemptLockTime    = "gs_unlock_attempt_lock_time";
        inline constexpr auto noLockTimeAttemptsLeft   = "gs_no_lock_time_attempts_left";
        inline constexpr auto displayLanguage          = "gs_display_language";
        inline constexpr auto inputLanguage            = "gs_input_language";
        inline constexpr auto automaticDateAndTimeIsOn = "gs_automatic_date_and_time_is_on";
        inline constexpr auto timeFormat               = "gs_time_format";
        inline constexpr auto dateFormat               = "gs_date_format";
        inline constexpr auto onboardingDone           = "gs_onboarding_done";
        inline constexpr auto eulaAccepted             = "gs_eula_accepted";
        inline constexpr auto osCurrentVersion         = "gs_os_current_version";
        inline constexpr auto osUpdateVersion          = "gs_os_update_version";
        inline constexpr auto currentTimezoneName      = "gs_current_timezone_name";
        inline constexpr auto currentTimezoneRules     = "gs_current_timezone_rules";
    } // namespace SystemProperties

    namespace Bluetooth
    {
        inline constexpr auto state            = "bt_state";
        inline constexpr auto deviceVisibility = "bt_device_visibility";
        inline constexpr auto deviceName       = "bt_device_name";
        inline constexpr auto bondedDevices    = "bt_bonded_devices";
        inline constexpr auto btKeys           = "bt_keys";
        inline constexpr auto connectedDevice  = "connected_device";
    } // namespace Bluetooth

    namespace Brightness
    {
        inline constexpr auto state                  = "br_state";
        inline constexpr auto brightnessLevel        = "br_level";
        inline constexpr auto bedsideBrightnessLevel = "br_bedside_level";
        inline constexpr auto bedsideTime            = "br_bedside_time";
        inline constexpr auto gammaFactor            = "br_gamma_Factor";
        inline constexpr auto autoMode               = "br_auto_mode";
    } // namespace Brightness

    namespace Cellular
    {
        inline constexpr auto apn_list     = "cl_apn_list";
        inline constexpr auto offlineMode  = "cl_offline_mode";
        inline constexpr auto currentUID   = "cl_current_uid";
        inline constexpr auto volteEnabled = "cl_volte_enabled";
    } // namespace Cellular

    namespace Battery
    {
        inline constexpr auto batteryCriticalLevel = "battery_critical_level";
    } // namespace Battery

    namespace Offline
    {
        inline constexpr auto connectionFrequency     = "msg_only_mode_connection_frequency";
        inline constexpr auto notificationsWhenLocked = "off_notifications_when_locked";
        inline constexpr auto callsFromFavorites      = "off_calls_from_favorites";
    } // namespace Offline

    namespace KeypadLight
    {
        inline constexpr auto state = "keypad_light_state";
    } // namespace KeypadLight

    namespace Wallpaper
    {
        inline constexpr auto option = "wallpaper_option";
    } // namespace Wallpaper

    namespace Quotes
    {
        inline constexpr auto randomQuotesList        = "quotes_random_list";
        inline constexpr auto randomQuoteIDUpdateTime = "quotes_random_id_update_time";
        inline constexpr auto selectedGroup           = "quotes_group";
    } // namespace Quotes

    namespace Display
    {
        inline constexpr auto invertedMode              = "display_inverted_mode";
        inline constexpr auto lockScreenDeepRefreshRate = "display_lock_screen_deep_refresh_rate";
    } // namespace Display

    namespace Meditation
    {
        inline constexpr auto duration        = "meditation_duration";
        inline constexpr auto intervalChime   = "meditation_interval_chime";
        inline constexpr auto preparationTime = "meditation_preparation_time";
        inline constexpr auto showCounter     = "meditation_show_counter";
    } // namespace Meditation
} // namespace settings
