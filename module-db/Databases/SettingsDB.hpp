
/*
 * @file SettingsDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Database/Database.hpp"
#include "../Tables/SettingsTable.hpp"
#include "../Tables/SettingsTable_v2.hpp"

class SettingsDB : public Database
{
  public:
    SettingsDB();
    ~SettingsDB() override = default;

    SettingsTable settings;
    SettingsTable_v2 settings_v2;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
