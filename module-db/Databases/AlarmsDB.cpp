// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vfs.hpp>
#include "AlarmsDB.hpp"

const char *AlarmsDB::dbName = USER_PATH("alarms.db");

AlarmsDB::AlarmsDB() : Database(dbName), alarms(this), alarmStatuses(this)
{}
