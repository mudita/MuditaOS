-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('system/phone_mode', 'offline'),
    ('system/phone_mode', 'dnd');

-- ----------- insert default values -------------------
INSERT OR REPLACE INTO settings_tab (path, value) VALUES
    ('system/phone_mode', 'online'),
    ('timeFormat12', '0'),
    ('timeDateFormat', '1'),
    ('activeSim', '1'),
    ('lockPassHash', '0'),
    ('lockTime', '30000'),
    ('displayLanguage', 'En'),
    ('inputLanguage', 'En');
