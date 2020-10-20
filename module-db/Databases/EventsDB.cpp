// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsDB.hpp"

#include <module-vfs/vfs.hpp>

const char *EventsDB::dbName = USER_PATH("events.db");

EventsDB::EventsDB() : Database(dbName), events(this)
{
    if (events.create() == false) {
        return;
    }
    isInitialized_ = true;
}
