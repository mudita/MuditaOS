-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

BEGIN TRANSACTION;
INSERT INTO "sms" ("_id","thread_id","contact_id","date","date_send","error_code","body","type") VALUES (5,1,1,1618907731,3,2,'Hi! How are you today?',8);
INSERT INTO "sms" ("_id","thread_id","contact_id","date","date_send","error_code","body","type") VALUES (6,1,1,1618907732,3,2,'I hope you''re feeling better...',2);
INSERT INTO "sms" ("_id","thread_id","contact_id","date","date_send","error_code","body","type") VALUES (7,2,2,1547492320,3,0,'Thank you for today!(/n) You chose a fantastic place :)',2);
INSERT INTO "sms" ("_id","thread_id","contact_id","date","date_send","error_code","body","type") VALUES (8,2,2,1547492320,4,0,'It was great seeing you too :*',4);
INSERT INTO "sms" ("_id","thread_id","contact_id","date","date_send","error_code","body","type") VALUES (14,1,1,161891,3,2,'Thanks :) Today is better than yestarday',4);
INSERT INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (1,'Thanks for reaching out. I can''t talk right now, I''ll call you later',4);
INSERT INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (2,'I''ll call you later',3);
INSERT INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (3,'I''ll be there in 15 minutes',2);
INSERT INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (4,'Some test template number, which is too long to be displayed.',1);
INSERT INTO "templates" ("_id","text","lastUsageTimestamp") VALUES (5,'Give me 5 minutes',5);
INSERT INTO "threads" ("_id","date","msg_count","read","contact_id","number_id","snippet","last_dir") VALUES (1,1574335694,6,1,1,1,'Hi! How are you today?',1);
INSERT INTO "threads" ("_id","date","msg_count","read","contact_id","number_id","snippet","last_dir") VALUES (2,1618910720,2,0,2,2,'Thank you for today',2);
INSERT INTO "threads" ("_id","date","msg_count","read","contact_id","number_id","snippet","last_dir") VALUES (6,1618910720,1,0,6,6,'Enjoy your digital detox!',8);
COMMIT;
