// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsDB.hpp"

EventsDB::EventsDB(const char *name) : Database(name), events(this)
{
    if (events.create() == false) {
        return;
    }
    isInitialized_ = true;
}
