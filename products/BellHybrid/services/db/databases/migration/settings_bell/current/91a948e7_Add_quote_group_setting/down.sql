-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add quote group setting
-- Revision: 91a948e7-4cff-4492-b25c-e219cfb4e8de
-- Create Date: 2025-01-27 08:55:40

-- Insert SQL here

DELETE FROM settings_tab WHERE path = 'quotes_group';
