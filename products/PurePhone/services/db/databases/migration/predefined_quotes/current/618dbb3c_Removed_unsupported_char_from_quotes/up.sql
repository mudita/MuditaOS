-- Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Message: Removed unsupported char from quotes
-- Revision: 618dbb3c-ea59-48d7-b882-9c568fd19905
-- Create Date: 2023-06-28 12:58:34

-- Insert SQL here

UPDATE OR IGNORE quote_table SET author='Línji Yixuán' WHERE author='Línjì Yìxuán'
