BEGIN TRANSACTION;
INSERT OR REPLACE INTO calls ("_id","number","e164number","presentation","date","duration","type","name","contactId","isRead") VALUES (1,'500466048','+48500466048',1,'2020-09-14 11:21:02',0,1,'alek',4,1);
COMMIT;
