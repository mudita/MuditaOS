// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "module-db/Tables/NotificationsTable.hpp"

class NotificationsDB : public Database
{
  public:
    NotificationsDB(const char *name);
    virtual ~NotificationsDB() = default;

    NotificationsTable notifications;
};
