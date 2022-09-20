-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS notifications(
    _id INTEGER PRIMARY KEY,
    key INTEGER UNIQUE DEFAULT 0,
    value INTEGER DEFAULT 0,
    contact_id INTEGER DEFAULT 0
);
