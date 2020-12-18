// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace settings
{
    namespace SystemProperties
    {
        constexpr inline auto timeFormat12    = "gs_time_format_12";
        constexpr inline auto timeDateFormat  = "gs_time_date_format";
        constexpr inline auto activeSim       = "gs_active_sim";
        constexpr inline auto lockPassHash    = "gs_lock_pass_hash";
        constexpr inline auto lockTime        = "gs_lock_time";
        constexpr inline auto displayLanguage = "gs_display_language";
        constexpr inline auto inputLanguage   = "gs_input_language";
    } // namespace SystemProperties
    namespace Bluetooth
    {
        constexpr inline auto state            = "bt_state";
        constexpr inline auto deviceVisibility = "bt_device_visibility";
        constexpr inline auto deviceName       = "bt_device_name";
        constexpr inline auto bondedDevices    = "bt_bonded_devices";
    } // namespace Bluetooth
    namespace Battery
    {
        constexpr inline auto criticalLevel = "batt_critical_level";
    } // namespace Battery

}; // namespace settings
