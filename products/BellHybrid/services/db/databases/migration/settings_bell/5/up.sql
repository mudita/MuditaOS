-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add quote group setting
-- Revision: 91a948e7-4cff-4492-b25c-e219cfb4e8de
-- Create Date: 2025-01-27 08:55:40

-- Insert SQL here

INSERT OR IGNORE INTO settings_tab (path, value) VALUES
       ('quotes_group', 'Predefined');

-- Message: Add quote interval setting
-- Revision: b6dd0dff-d717-4bf7-97e8-cc3e6359d9ff
-- Create Date: 2025-01-30 13:47:07

-- Insert SQL here

INSERT OR IGNORE INTO settings_tab (path, value) VALUES
       ('quotes_interval', 'AtMidnight');

