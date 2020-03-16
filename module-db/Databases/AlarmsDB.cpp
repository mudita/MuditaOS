/*
 * AlarmsDB.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include "AlarmsDB.hpp"

const char *AlarmsDB::dbName = "sys/alarms.db";

AlarmsDB::AlarmsDB() : Database(dbName), alarms(this)
{

    if (alarms.Create() == false)
        return;

    isInitialized = true;
}

AlarmsDB::~AlarmsDB()
{}
