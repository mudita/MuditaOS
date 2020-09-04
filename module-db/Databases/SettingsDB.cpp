
/*
 * @file SettingsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 05.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
#include "SettingsDB.hpp"
const char *SettingsDB::dbName = USER_PATH("settings.db");

SettingsDB::SettingsDB() : Database(dbName), settings(this), settings_v2(this)
{
    if (settings.create() == false || settings_v2.create() == false)
        return;

    isInitialized_ = true;
}
