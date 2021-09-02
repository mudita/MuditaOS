-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('system/phone_mode', 'offline'),
    ('system/phone_mode', 'dnd');

-- ----------- insert default values -------------------
INSERT OR IGNORE INTO settings_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('gs_time_format', '0'),
    ('gs_date_format', '1'),
    ('gs_active_sim', 'SIM1'),
    ('gs_lock_pass_hash', '3333'),
    ('gs_lock_screen_passcode_is_on', '1'),
    ('gs_lock_time', '30'),
    ('gs_display_language', 'English'),
    ('gs_input_language', 'English'),
    ('gs_eula_accepted', '0'),
    ('\ApplicationManager\\gs_onboarding_done', '1'),
    ('gs_usb_security', '1'),
    ('gs_os_update_version', '0.00.0'),
    ('gs_os_current_version', '0.00.0'),
    ('\ServiceBluetooth\\bt_state', '0'),
    ('\ServiceBluetooth\\bt_device_visibility', '0'),
    ('\ServiceBluetooth\\bt_device_name', 'PurePhone'),
    ('\ServiceBluetooth\\bt_bonded_devices', ''),
    ('battery_critical_level', '10'),
    ('cl_offline_mode', '1'),
    ('off_connection_frequency', '0'),
    ('off_notifications_when_locked', '0'),
    ('off_calls_from_favorites', '0'),
    ('\EventManager\\br_state', '0'),
    ('\EventManager\\br_auto_mode', '0'),
    ('\EventManager\\br_level', '50.0f'),
    ('keypad_light_state', '0'),
    ('gs_current_timezone_name', ''),
    ('gs_current_timezone_rules', ''),
    ('\ServiceTime\\gs_automatic_date_and_time_is_on', '1');


