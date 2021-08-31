-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
BEGIN TRANSACTION;
INSERT OR REPLACE INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (1,'Thanks for reaching out. I can''t talk right now, I''ll call you later',4);
INSERT OR REPLACE INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (2,'I''ll call you later',3);
INSERT OR REPLACE INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (3,'I''ll be there in 15 minutes',2);
INSERT OR REPLACE INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (4,'Give me 5 minutes',5);
COMMIT;
