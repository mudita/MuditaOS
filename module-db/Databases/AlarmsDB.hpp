/*
 * AlarmsDB.hpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#pragma once
#include "Database/Database.hpp"
#include "../Tables/AlarmsTable.hpp"
#include "../Tables/ThreadsTable.hpp"

class AlarmsDB : public Database
{
  public:
    AlarmsDB();
    ~AlarmsDB();

    AlarmsTable alarms;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
