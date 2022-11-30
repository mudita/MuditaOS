-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (1, 'Off');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (2, 'On');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (3, 'FirstSnooze');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (4, 'SecondSnooze');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (5, 'ThirdSnooze');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (6, 'FourthSnooze');
INSERT or IGNORE INTO alarmStatuses (_id, name) VALUES (7, 'FifthSnooze');
