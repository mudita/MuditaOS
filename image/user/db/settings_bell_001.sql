-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

