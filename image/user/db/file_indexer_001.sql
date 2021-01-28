-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
