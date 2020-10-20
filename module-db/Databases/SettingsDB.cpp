// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-vfs/vfs.hpp>
#include "SettingsDB.hpp"
const char *SettingsDB::dbName = USER_PATH("settings.db");

SettingsDB::SettingsDB() : Database(dbName), settings(this), settings_v2(this)
{
    if (settings.create() == false || settings_v2.create() == false)
        return;

    isInitialized_ = true;
}
