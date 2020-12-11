// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Database/Database.hpp"
#include "Tables/AlarmsTable.hpp"
#include "Tables/AlarmsStatusTable.hpp"

class AlarmsDB : public Database
{
  public:
    AlarmsDB(const char *name);
    ~AlarmsDB() = default;

    AlarmsTable alarms;
    AlarmsStatusTable alarmStatuses;
};
