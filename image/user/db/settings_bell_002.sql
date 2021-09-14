-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('system/phone_mode', 'offline'),
    ('system/phone_mode', 'dnd');

-- ----------- insert default values -------------------
INSERT OR IGNORE INTO settings_tab (path, value) VALUES
    ('gs_time_format', '0'),
    ('gs_date_format', '1'),
    ('gs_display_language', 'English'),
    ('gs_input_language', 'English'),
    ('\ApplicationManager\\gs_onboarding_done', '0'),
    ('gs_usb_security', '1'),
    ('gs_os_update_version', '0.00.0'),
    ('gs_os_current_version', '0.00.0'),
    ('battery_critical_level', '10'),
    ('cl_offline_mode', '0'),
    ('cl_current_uid', '0'),
    ('off_connection_frequency', '0'),
    ('off_notifications_when_locked', '0'),
    ('off_calls_from_favorites', '0'),
    ('\EventManager\\br_state', '0'),
    ('\EventManager\\br_auto_mode', '0'),
    ('\EventManager\\br_level', '50.0f'),
    ('keypad_light_state', '0'),
    ('gs_current_timezone_name', ''),
    ('gs_current_timezone_rules', ''),
    ('\ServiceTime\\gs_automatic_date_and_time_is_on', '1'),
    ('temperature_unit', 'C'),
    ('ringing_duration', '10000'),
    ('ringing_tone', 'Nick_Lewis_-_Kristies_Elephant.mp3'),
    ('snooze_active', '1'),
    ('snooze_length','10'),
    ('snooze_interval','1'),
    ('snooze_tone','Meditative surprises'),
    ('snooze_volume','10'),
    ('prewake_up_duration', '10'),
    ('prewake_up_tone','Meditative surprises'),
    ('prewake_up_volume','5'),
    ('prewake_up_light_duration','10');
