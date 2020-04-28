/*
 * AlarmsDB.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include <module-vfs/vfs.hpp>
#include "AlarmsDB.hpp"

const char *AlarmsDB::dbName = USER_PATH("alarms.db");

AlarmsDB::AlarmsDB() : Database(dbName), alarms(this)
{

    if (alarms.Create() == false)
        return;

    isInitialized = true;
}

AlarmsDB::~AlarmsDB()
{}
