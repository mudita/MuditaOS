-- Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

CREATE TABLE IF NOT EXISTS sms_alert_overrides(
                                                   _id INTEGER PRIMARY KEY,
                                                   normalized_number TEXT NOT NULL UNIQUE,
                                                   sound_path TEXT NOT NULL DEFAULT '',
                                                   ignore_dnd INTEGER NOT NULL DEFAULT 0,
                                                   force_vibration INTEGER NOT NULL DEFAULT 0,
                                                   force_max_volume INTEGER NOT NULL DEFAULT 0,
                                                   repeat_interval_sec INTEGER NOT NULL DEFAULT 30,
                                                   enabled INTEGER NOT NULL DEFAULT 1
);
