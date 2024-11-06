-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Change store brightness from percent to level
-- Revision: 287a0959-d838-49a9-bc3c-a25b5969d8a1
-- Create Date: 2024-11-06 11:20:45

-- The transformation equation has been derived from FrontlightUtils.hpp helpers

UPDATE OR IGNORE settings_tab
SET value = (SELECT CAST(ROUND((value - 16.0) / ((100.0 - 16.0) / 10.0) + 1.0) AS INT)
    FROM settings_tab
    WHERE path = 'br_level')
WHERE path = 'br_level';

UPDATE OR IGNORE settings_tab
SET value = (SELECT CAST(ROUND((value - 16.0) / ((100.0 - 16.0) / 10.0) + 1.0) AS INT)
    FROM settings_tab
    WHERE path = 'br_bedside_level')
WHERE path = 'br_bedside_level';

UPDATE OR IGNORE settings_tab
SET value = (SELECT CAST(ROUND((value - 16.0) / ((100.0 - 16.0) / 10.0) + 1.0) AS INT)
    FROM settings_tab
    WHERE path = 'alarm_brightness')
WHERE path = 'alarm_brightness';

UPDATE OR IGNORE settings_tab
SET value = (SELECT CAST(ROUND((value - 16.0) / ((100.0 - 16.0) / 10.0) + 1.0) AS INT)
    FROM settings_tab
    WHERE path = 'prewake_up_brightness')
WHERE path = 'prewake_up_brightness';
