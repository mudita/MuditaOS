-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS alarms(
                  _id INTEGER PRIMARY KEY,
                  time DATETIME,
                  snooze INTEGER,
                  status INTEGER,
                  repeat INTEGER,
                  path TEXT DEFAULT '',
                  FOREIGN KEY(status) REFERENCES alarmStatuses(_id)
);

CREATE TABLE IF NOT EXISTS alarmStatuses(
                  _id INTEGER PRIMARY KEY,
                  name TEXT NOT NULL
);
