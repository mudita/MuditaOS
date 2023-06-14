-- Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Adding new column to templates table to manage templates order
-- Revision: 111a2d1b-dc32-40a3-9a2a-02a9e186bcac
-- Create Date: 2023-03-23 10:11:44

ALTER TABLE templates ADD rowOrder INTEGER;

UPDATE OR IGNORE templates SET rowOrder = _id;

