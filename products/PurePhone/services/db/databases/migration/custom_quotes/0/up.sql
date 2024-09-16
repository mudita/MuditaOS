-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

CREATE TABLE IF NOT EXISTS quote_table(
                                          quote_id INTEGER PRIMARY KEY,
                                          quote TEXT NOT NULL,
                                          author TEXT,
                                          enabled BOOLEAN NOT NULL DEFAULT TRUE
);
