-- Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Move gs_onboarding_done to global scope
-- Revision: 34a5f438-a1fe-4818-af74-d084dc67ddf2
-- Create Date: 2023-03-02 12:44:53

UPDATE OR IGNORE settings_tab
SET path = '\ApplicationManager\\gs_onboarding_done'
WHERE path='gs_onboarding_done';

