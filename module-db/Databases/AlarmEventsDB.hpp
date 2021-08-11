// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Database/Database.hpp"
#include "Tables/AlarmEventsTable.hpp"

class AlarmEventsDB : public Database
{
  public:
    AlarmEventsDB(const char *name);
    ~AlarmEventsDB() = default;

    AlarmEventsTable alarmEvents;
};
