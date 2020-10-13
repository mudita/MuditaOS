#pragma once

#include "module-db/Database/Database.hpp"
#include "module-db/Tables/EventsTable.hpp"

class EventsDB : public Database
{
  public:
    EventsDB();
    ~EventsDB() override = default;

    EventsTable events;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
