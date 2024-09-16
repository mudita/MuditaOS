-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

CREATE TABLE IF NOT EXISTS notes(
                                    _id INTEGER PRIMARY KEY,
                                    date INTEGER,
                                    snippet TEXT DEFAULT ''
);
