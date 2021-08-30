-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS files
(
    _id         INTEGER PRIMARY KEY,
    path        TEXT,
    media_type  TEXT,
    size        INTEGER,
    title       TEXT,
    artist      TEXT,
    album       TEXT,
    comment     TEXT,
    genre       TEXT,
    year        INTEGER,
    track       INTEGER,
    song_length INTEGER,
    bitrate     INTEGER,
    sample_rate INTEGER,
    channels    INTEGER
);
