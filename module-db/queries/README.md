Virtual database query interface.
================================

Instead, manually creating switch cases for each possible query logic, create one 
virtual `query` interface and push `querying` logic to db (where it's implemented)

* db::service accepts `GetByQuery(service, database, query)`
* it routes it to proper database on request (instead manually copying interfaces)
* db implements querying logics and returns response
