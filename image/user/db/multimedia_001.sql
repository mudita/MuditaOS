-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS files
(
    _id         INTEGER PRIMARY KEY,
    path        TEXT,       /* filepath */
    media_type  TEXT,       /* mime type e.g. "audio/mp3" */
    size        INTEGER,    /* file size in bytes */
    title       TEXT,       /* song title */
    artist      TEXT,       /* song artist */
    album       TEXT,       /* song album */
    comment     TEXT,       /* comment */
    genre       TEXT,       /* e.g. "blues, classic rock" */
    year        INTEGER,    /* year of release */
    track       INTEGER,    /* track number */
    song_length INTEGER,    /* length of the song in seconds */
    bitrate     INTEGER,    /* bitrate of the song in kb/s */
    sample_rate INTEGER,    /* sample rate of the song in Hz */
    channels    INTEGER     /* number of channels 1 - mono, 2 - stereo */
);
