-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS calls(
                                    _id INTEGER PRIMARY KEY,
                                    number TEXT DEFAULT '',
                                    e164number TEXT DEFAULT '',
                                    presentation INTEGER DEFAULT 0,
                                    date INTEGER DEFAULT 0,
                                    duration INTEGER DEFAULT 0,
                                    type INTEGER DEFAULT 0,
                                    name TEXT DEFAULT '',
                                    contactId INTEGER DEFAULT 0,
                                    isRead INTEGER DEFAULT 1
);
-- calls.contactId should not be used.
-- calls.name should not be used.
