
/*
 * @file SettingsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "SettingsDB.hpp"

const char *SettingsDB::dbName = "/user/settings.db";

SettingsDB::SettingsDB() : Database(dbName), settings(this)
{

    if (settings.Create() == false)
        return;

    isInitialized = true;
}

SettingsDB::~SettingsDB()
{}