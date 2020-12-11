// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "module-db/Tables/EventsTable.hpp"

class EventsDB : public Database
{
  public:
    EventsDB(const char *name);
    ~EventsDB() override = default;

    EventsTable events;
};
