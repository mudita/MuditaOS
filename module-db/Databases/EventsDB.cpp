#include "EventsDB.hpp"

#include <module-vfs/vfs.hpp>

const char *EventsDB::dbName = USER_PATH("events.db");

EventsDB::EventsDB() : Database(dbName), events(this)
{
    if (events.create() == false)
        return;
    isInitialized_ = true;
}
