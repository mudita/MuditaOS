-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

R"dbInitStr(

/* Create File indexer  tables */
BEGIN TRANSACTION;

-- Main file table
CREATE TABLE IF NOT EXISTS file_tab (
    file_id INTEGER,
    path TEXT NOT NULL UNIQUE,
    size INTEGER,
    mime_type INTEGER,
    mtime INTEGER,
    directory TEXT,
    file_type INTEGER,
    PRIMARY KEY (file_id),
    CONSTRAINT file_path_unique
        UNIQUE (file_id, path) ON CONFLICT REPLACE
    );

-- Table contains information
-- about file metadata.
CREATE TABLE IF NOT EXISTS metadata_tab (
    file_id INTEGER NOT NULL,
    property TEXT NOT NULL,
    value TEXT NOT NULL,
    PRIMARY KEY (file_id, property),
    FOREIGN KEY (file_id) REFERENCES file_tab(file_id)
    );

-- Table contains information who to inform
-- about changes in values.
CREATE TABLE IF NOT EXISTS notifications_tab (
    id INTEGER PRIMARY KEY,
    path TEXT NOT NULL,
    service TEXT,
    CONSTRAINT notification_unique
         UNIQUE(path, service)
);

-- ----------- insert default values ----------------------

INSERT OR REPLACE INTO file_tab (file_id, path, size, mime_type, mtime, directory, file_type) VALUES
    (1, 'mp3/track1.mp3', 456666, 1, 1603929600, 'mp3',12297),
    (2, 'mp3/track2.mp3', 345354 ,1, 1603929604, 'mp3',12297),
    (3, 'mp3/track3.mp3', 34534 ,1, 1603929604, 'mp3',12297),
    (4, 'mp3/track4.mp3', 345354 ,1, 1603929604, 'mp3',12297);


INSERT OR REPLACE INTO metadata_tab (file_id, property, value) VALUES
    (1,'artist','Sting'),
    (1,'genre','Rock'),
    (1,'album','Album1'),
    (2,'artist','Madonna'),
    (2,'genre','Rock'),
    (2,'album','Album2'),
    (3,'artist','Lady Gaga'),
    (3,'genre','Rock'),
    (3,'album','Album3'),
    (4,'artist','Nick Levis'),
    (4,'genre','Soul'),
    (4,'album','Album4');

INSERT OR REPLACE INTO notifications_tab (id, path, service) VALUES
    (1,'mp3','ServiceDB');

COMMIT;

)dbInitStr"
