-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add fix for update from 2 9 x
-- Revision: d1af203c-2982-4986-b680-fc7465019c4a
-- Create Date: 2025-08-07 17:30:39

-- Insert SQL here

DELETE FROM whats_new WHERE Major = 2 AND Minor = 10 AND Patch = 0 AND English_title = 'Custom quotations';
