#include "NotificationsDB.hpp"

#include <module-vfs/vfs.hpp>

const char *NotificationsDB::dbName = USER_PATH("notifications.db");

NotificationsDB::NotificationsDB() : Database(dbName), notifications(this)
{
    if (notifications.create() == false)
        return;
    isInitialized = true;
}
