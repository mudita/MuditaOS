-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('system/phone_mode', 'offline'),
    ('system/phone_mode', 'dnd');

-- ----------- insert default values -------------------
INSERT OR IGNORE INTO settings_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('gs_time_format_12', '0'),
    ('gs_time_date_format', '1'),
    ('gs_active_sim', 'SIM1'),
    ('gs_lock_pass_hash', '3333'),
    ('gs_lock_time', '30000'),
    ('gs_display_language', 'English'),
    ('gs_input_language', 'English');
    ('bt_state', '0'),
    ('bt_device_visibility', '0'),
    ('bt_device_name', 'PurePhone'),
    ('bt_bonded_devices', '');
