CREATE TABLE IF NOT EXISTS alarms
(
    _id    INTEGER PRIMARY KEY,
    time   INTEGER,
    snooze INTEGER,
    status INTEGER,
    path   TEXT DEFAULT ''
);
