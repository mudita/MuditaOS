#pragma once

#include "Database/Database.hpp"
#include "module-db/Tables/EventsTable.hpp"

class EventsDB : public Database
{
  public:
    EventsDB();
    virtual ~EventsDB() = default;

    EventsTable events;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
