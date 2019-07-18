/*
 * AlarmsTable.hpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#ifndef MODULE_DB_TABLES_ALARMSTABLE_HPP_
#define MODULE_DB_TABLES_ALARMSTABLE_HPP_

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"


struct AlarmsTableRow{
    uint32_t ID;
    uint32_t time;
    uint32_t snooze;
    uint32_t status;
    UTF8    path;
};

enum class AlarmsTableFields{
    Time,
    Snooze,
    Status,
	Path
};

class AlarmsTable : public Table<AlarmsTableRow,AlarmsTableFields> {
public:

	AlarmsTable(Database* db);
    ~AlarmsTable();

    bool Create() override final;
    bool Add(AlarmsTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(AlarmsTableFields field, const char* str) override final;
    bool Update(AlarmsTableRow entry) override final;
    AlarmsTableRow GetByID(uint32_t id) override final;
    std::vector<AlarmsTableRow> GetLimitOffset(uint32_t offset,uint32_t limit) override final;
    std::vector<AlarmsTableRow> GetLimitOffsetByField(uint32_t offset,uint32_t limit,AlarmsTableFields field,const char* str) override final;

    uint32_t GetCount() override final;
    uint32_t GetCountByFieldID(const char* field,uint32_t id) override final;

    AlarmsTableRow GetNext(time_t time);

private:

    const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS alarms("
            "_id INTEGER PRIMARY KEY,"
            "time INTEGER,"
            "snooze INTEGER,"
            "status INTEGER,"
            "path TEXT DEFAULT '');";
    const char* alarmsInitialization =
                "INSERT OR IGNORE INTO alarms (time, snooze, status, path) VALUES(0,0,0,'')";


};

#endif /* MODULE_DB_TABLES_ALARMSTABLE_HPP_ */
