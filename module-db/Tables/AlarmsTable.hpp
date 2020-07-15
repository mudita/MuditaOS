/*
 * AlarmsTable.hpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */
#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct AlarmsTableRow
{
    uint32_t ID = DB_ID_NONE;
    uint32_t time;
    uint32_t snooze;
    uint32_t status;
    UTF8 path;
};

enum class AlarmsTableFields
{
    Time,
    Snooze,
    Status,
    Path
};

class AlarmsTable : public Table<AlarmsTableRow, AlarmsTableFields>
{
  public:
    AlarmsTable(Database *db);
    virtual ~AlarmsTable();

    bool create() override final;
    bool add(AlarmsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(AlarmsTableFields field, const char *str) override final;
    bool update(AlarmsTableRow entry) override final;
    AlarmsTableRow getById(uint32_t id) override final;
    std::vector<AlarmsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<AlarmsTableRow> getLimitOffsetByField(uint32_t offset,
                                                      uint32_t limit,
                                                      AlarmsTableFields field,
                                                      const char *str) override final;

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

    AlarmsTableRow GetNext(time_t time);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS alarms("
                                   "_id INTEGER PRIMARY KEY,"
                                   "time INTEGER,"
                                   "snooze INTEGER,"
                                   "status INTEGER,"
                                   "path TEXT DEFAULT '');";
    // "INSERT or ignore INTO alarms (_id, time, snooze, status, path ) VALUES (1,0,0,0,'');"
    const char *alarmsInitialization =
        "INSERT or ignore INTO alarms (_id, time, snooze, status, path ) VALUES (1,0,0,0,'');";
};
