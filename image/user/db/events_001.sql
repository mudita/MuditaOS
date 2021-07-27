-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS events(
                  _id INTEGER PRIMARY KEY,
                  name TEXT DEFAULT '',
                  startDate DATETIME,
                  endDate DATETIME,
                  duration INTEGER,
                  isAllDay BOOLEAN,
                  recurrenceRule TEXT DEFAULT ''
);
