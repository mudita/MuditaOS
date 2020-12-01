// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/SettingsTable_v2.hpp"

class SettingsDB : public Database
{
  public:
    SettingsDB();
    ~SettingsDB() override = default;

    SettingsTable_v2 settings_v2;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
