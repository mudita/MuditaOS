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
    ('msg_only_mode_connection_frequency', '15'),
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
    ('snooze_active', '1'),
    ('snooze_length','10'),
    ('snooze_interval','0'),
    ('snooze_tone','Gentle Chime'),
    ('prewake_up_duration', '0'),
    ('prewake_up_tone','Joyful Awakening'),
    ('prewake_up_light_duration','0'),
    ('prewake_up_brightness','50.0f'),
    ('alarm_tone','Morning Dew'),
    ('alarm_light_active','1'),
    ('alarm_duration','5'),
    ('alarm_brightness','50.0f'),
    ('bedtime_active','0'),
    ('bedtime_time','22:00'),
    ('bedtime_tone','Evening Horizon'),
    ('bedtime_duration','5'),
    ('layout','ClassicWithBattery'),
    ('\ServiceEink\\display_inverted_mode', '0');


