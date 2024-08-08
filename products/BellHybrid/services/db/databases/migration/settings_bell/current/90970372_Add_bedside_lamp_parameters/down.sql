-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Add bedside lamp parameters
-- Revision: 90970372-6151-4de3-91dd-f1c8684bc6f4
-- Create Date: 2024-08-08 13:56:09

DELETE FROM settings_tab WHERE path = 'br_bedside_level';
DELETE FROM settings_tab WHERE path = 'br_bedside_time';

UPDATE OR IGNORE settings_tab
SET path = '\EventManager\\br_state'
WHERE path = 'br_state';

UPDATE OR IGNORE settings_tab
SET path = '\EventManager\\br_auto_mode'
WHERE path = 'br_auto_mode';

UPDATE OR IGNORE settings_tab
SET path = '\EventManager\\br_level'
WHERE path = 'br_level';
