-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
BEGIN TRANSACTION;
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (1,2,2,1547492320,0,'Thank you for today!' || CHAR(10) || 'You chose a fantastic place :)',8);
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (2,2,2,1547492321,0,'It was great seeing you too :*',4);
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (3,1,1,1618907731,2,'Hi! How are you today?',8);
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (4,1,1,1618907732,2,'I hope you''re feeling better now...',8);
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (5,1,1,1618907733,2,'Thanks :) Today is better than yesterday',4);
INSERT OR REPLACE INTO "sms" ("_id","thread_id","contact_id","date","error_code","body","type") VALUES (6,1,1,1618907734,2,'I''m happy to hear that :)',8);
INSERT OR REPLACE INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (5,'Some test template number, which is too long to be displayed.',1);
INSERT OR REPLACE INTO "threads" ("_id","date","msg_count","read","contact_id","number_id","snippet","last_dir") VALUES (1,1618907734,3,1,1,1,'I''m happy to hear that :)',4);
INSERT OR REPLACE INTO "threads" ("_id","date","msg_count","read","contact_id","number_id","snippet","last_dir") VALUES (2,1547492321,2,0,2,2,'It was great seeing you too :*',4);
COMMIT;
