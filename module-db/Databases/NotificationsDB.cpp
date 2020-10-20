// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsDB.hpp"

#include <module-vfs/vfs.hpp>

const char *NotificationsDB::dbName = USER_PATH("notifications.db");

NotificationsDB::NotificationsDB() : Database(dbName), notifications(this)
{
    if (notifications.create() == false)
        return;
    isInitialized_ = true;
}
