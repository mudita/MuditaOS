-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('system/phone_mode', 'offline'),
    ('system/phone_mode', 'dnd');

-- ----------- insert default values -------------------
INSERT OR REPLACE INTO settings_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('gs_time_format_12', '0'),
    ('gs_time_date_format', '1'),
    ('gs_active_sim', '1'),
    ('gs_lock_pass_hash', '3333'),
    ('gs_lock_time', '30000'),
    ('gs_display_language', 'En'),
    ('gs_input_language', 'En');
