/*
 * Create Settings tables
 */


BEGIN TRANSACTION;
--
-- Main settings table, for string application persistent data
--
CREATE TABLE IF NOT EXISTS settings_tab (
    path TEXT NOT NULL UNIQUE,
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

-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO dictionary_tab (path, value) VALUES
    ("system/phone_mode", "online"),
    ("system/phone_mode", "offline"),
    ("system/phone_mode", "dnd");

INSERT OR REPLACE INTO settings_tab (path, value) VALUES
    ("system/phone_mode", "online");

COMMIT TRANSACTION;
