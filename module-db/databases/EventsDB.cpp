// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsDB.hpp"

EventsDB::EventsDB(const char *name) : Database(name), alarmEvents(this)
{}
