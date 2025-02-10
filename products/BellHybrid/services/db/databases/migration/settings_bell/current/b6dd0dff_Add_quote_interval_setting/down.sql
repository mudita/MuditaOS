-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add quote interval setting
-- Revision: b6dd0dff-d717-4bf7-97e8-cc3e6359d9ff
-- Create Date: 2025-01-30 13:47:07

-- Insert SQL here

DELETE FROM settings_tab WHERE path = 'quotes_interval';
