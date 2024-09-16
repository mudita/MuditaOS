-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

CREATE TABLE IF NOT EXISTS meditation_stats
(
    _id         INTEGER PRIMARY KEY,
    timestamp   TEXT,
    duration    INTEGER
);
