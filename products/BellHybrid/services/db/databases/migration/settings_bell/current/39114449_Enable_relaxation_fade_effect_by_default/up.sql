-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Enable relaxation fade effect by default
-- Revision: 39114449-29a6-42a3-a239-57b8c726501c
-- Create Date: 2024-09-05 11:37:44

-- Insert SQL here

INSERT OR IGNORE INTO settings_tab (path, value) VALUES
       ('relaxation_fade_active', '1');
