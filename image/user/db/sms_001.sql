-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS sms
(
    _id        INTEGER PRIMARY KEY,
    thread_id  INTEGER,
    contact_id INTEGER,
    date       INTEGER,
    error_code INTEGER,
    body       TEXT NOT_NULL,
    type       INTEGER,
    FOREIGN KEY (thread_id) REFERENCES threads (_id) ON DELETE CASCADE
);
-- sms.contact_id should not be used.

CREATE TABLE IF NOT EXISTS templates
(
    _id                INTEGER PRIMARY KEY,
    text               TEXT,
    lastUsageTimestamp INTEGER
);

CREATE TABLE IF NOT EXISTS threads
(
    _id        INTEGER PRIMARY KEY,
    date       INTEGER,
    msg_count  INTEGER,
    read       INTEGER,
    contact_id INTEGER,
    number_id  INTEGER,
    snippet    TEXT NOT NULL,
    last_dir   INTEGER
);
-- threads.contact_id should not be used.

CREATE TABLE IF NOT EXISTS threads_count
(
    _id   INTEGER PRIMARY KEY,
    count INTEGER
);

CREATE TRIGGER IF NOT EXISTS on_thread_insert AFTER INSERT ON threads BEGIN UPDATE threads_count SET count=count+1 WHERE _id=1; END;
CREATE TRIGGER IF NOT EXISTS on_thread_remove AFTER DELETE ON threads BEGIN UPDATE threads_count SET count=count-1 WHERE _id=1; END;
