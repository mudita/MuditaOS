-- Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Move gs_onboarding_done to global scope
-- Revision: 06e96a01-c32c-49b7-8909-43b310f181ff
-- Create Date: 2023-03-02 13:41:10

UPDATE OR IGNORE settings_tab
SET path = 'gs_onboarding_done'
WHERE path='\ApplicationManager\\gs_onboarding_done';

