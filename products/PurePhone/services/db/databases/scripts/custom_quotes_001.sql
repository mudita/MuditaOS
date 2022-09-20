-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS quote_table(
    quote_id INTEGER PRIMARY KEY,
    quote TEXT NOT NULL,
    author TEXT,
    enabled BOOLEAN NOT NULL DEFAULT TRUE
);
