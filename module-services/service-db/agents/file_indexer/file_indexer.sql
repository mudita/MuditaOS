-- Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
/*
 * Create File indexer  tables
*/

BEGIN TRANSACTION;

--
-- File type table,
--
CREATE TABLE IF NOT EXISTS file_type_tab (
    file_type_id INTEGER NOT NULL,
    mime_type TEXT NOT NULL,
    PRIMARY KEY (file_type_id)
);


--
-- Main file table
--
CREATE TABLE IF NOT EXISTS file_tab (
    file_id INTEGER NOT NULL,
    path TEXT NOT NULL UNIQUE,
    size TEXT,
    file_type_id INTEGER,
    PRIMARY KEY (file_id),
    FOREIGN KEY( file_type_id) REFERENCES file_type_tab(file_type_id)
    );

--
-- Table contains information
-- about file metadata.
--
CREATE TABLE IF NOT EXISTS metadata_tab (
    file_id INTEGER NOT NULL,
    property TEXT NOT NULL,
    value TEXT NOT NULL,
    PRIMARY KEY (file_id, property)
    );


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
INSERT OR REPLACE INTO file_tab (path, size) VALUES
    ('audio/track1.mp3', '4567'),
    ('audio/track2.mp3', '345354');

INSERT OR REPLACE INTO file_type_tab (mime_type) VALUES
    ('audio/mpeg'),
    ('audio/x-wav'),
    ('audio/basic'),
    ('audio/midi'),
    ('audio/x-aiff');

INSERT OR REPLACE INTO metadata_tab (file_id, property, value) VALUES
    (1,'artist','Sting'),
    (1,'genre','Rock'),
    (1,'album','Album1'),
    (2,'artist','Nick Levis'),
    (2,'genre','Soul'),
    (2,'album','Album2');


COMMIT;

