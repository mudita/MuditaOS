// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

    if (alarms.create() == false)
        return;

    isInitialized_ = true;
}

AlarmsDB::~AlarmsDB()
{}
