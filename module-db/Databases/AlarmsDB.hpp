/*
 * AlarmsDB.hpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#ifndef MODULE_DB_DATABASES_ALARMSDB_HPP_
#define MODULE_DB_DATABASES_ALARMSDB_HPP_

#include "Database/Database.hpp"
#include "../Tables/AlarmsTable.hpp"
#include "../Tables/ThreadsTable.hpp"

class AlarmsDB : public Database {
public:
    AlarmsDB();
    ~AlarmsDB();

    AlarmsTable alarms;
    ThreadsTable threads;

    static const char* GetDBName(){return dbName;}

private:

    static const char *dbName;
};




#endif /* MODULE_DB_DATABASES_ALARMSDB_HPP_ */
