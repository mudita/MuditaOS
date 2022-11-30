-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

--
-- Main settings table, for string application persistent data
--
CREATE TABLE IF NOT EXISTS settings_tab (
                                            path TEXT NOT NULL UNIQUE PRIMARY KEY,
                                            value TEXT
);

--
-- Dictionary table, for variables with fixed set of values
--
CREATE TABLE IF NOT EXISTS dictionary_tab (
                                              id INTEGER PRIMARY KEY,
                                              path TEXT NOT NULL,
                                              value TEXT,
                                              CONSTRAINT dictionary_unique
                                              UNIQUE (path, value) ON CONFLICT REPLACE
    );

--
-- Table contains information who to inform
-- about changes in values.
--
CREATE TABLE IF NOT EXISTS notifications_tab (
                                                 id INTEGER PRIMARY KEY,
                                                 path TEXT NOT NULL,
                                                 service TEXT,
                                                 CONSTRAINT notification_unique
                                                 UNIQUE(path, service)
    );

CREATE TABLE IF NOT EXISTS settings_changed_tab(

                                                   id INTEGER PRIMARY KEY,
                                                   path TEXT NOT NULL,
                                                   value TEXT,
                                                   CONSTRAINT changed_unique
                                                   UNIQUE(path ) ON CONFLICT REPLACE
    );

CREATE TRIGGER IF NOT EXISTS on_update UPDATE OF value ON settings_tab
    WHEN new.value <> old.value BEGIN INSERT OR REPLACE INTO  settings_changed_tab (path, value) VALUES (new.path,new.value); END;

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
    ('gs_active_sim', ''),
    ('gs_auto_lock_time', '30'),
    ('gs_unlock_lock_time', '0'),
    ('gs_unlock_attempt_lock_time', '0'),
    ('gs_no_lock_time_attempts_left', '3'),
    ('gs_lock_pass_hash', ''),
    ('gs_lock_screen_passcode_is_on', '0'),
    ('gs_display_language', 'English'),
    ('gs_input_language', 'English'),
    ('gs_eula_accepted', '0'),
    ('\ApplicationManager\\gs_onboarding_done', '0'),
    ('gs_usb_security', '1'),
    ('gs_os_update_version', '0.00.0'),
    ('gs_os_current_version', '0.00.0'),
    ('\ServiceBluetooth\\bt_state', '0'),
    ('\ServiceBluetooth\\bt_device_visibility', '0'),
    ('\ServiceBluetooth\\bt_device_name', 'Mudita Pure'),
    ('\ServiceBluetooth\\bt_bonded_devices', ''),
    ('battery_critical_level', '10'),
    ('cl_offline_mode', '1'),
    ('cl_current_uid', '0'),
    ('msg_only_mode_connection_frequency', '15'),
    ('off_notifications_when_locked', '0'),
    ('off_calls_from_favorites', '0'),
    ('\EventManager\\br_state', '0'),
    ('\EventManager\\br_auto_mode', '0'),
    ('\EventManager\\br_level', '50.0f'),
    ('keypad_light_state', '0'),
    ('wallpaper_option', '0'),
    ('gs_current_timezone_name', ''),
    ('gs_current_timezone_rules', ''),
    ('\ServiceTime\\gs_automatic_date_and_time_is_on', '1'),
    ('\ServiceEink\\display_inverted_mode', '0'),
    ('display_lock_screen_deep_refresh_rate', '30'),
    ('cl_volte_enabled', '0');



