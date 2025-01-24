-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add a new table for custom quotes
-- Revision: 87d133bf-f98c-429d-9f15-6963fd72f7e1
-- Create Date: 2025-01-23 15:32:56

-- Insert SQL here
CREATE TABLE IF NOT EXISTS custom_quote_table (
    quote_id INTEGER NOT NULL,
    quote TEXT NOT NULL,
    author TEXT NOT NULL,
    PRIMARY KEY (quote_id)
);
