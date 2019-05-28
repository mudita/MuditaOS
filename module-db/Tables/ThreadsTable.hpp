
/*
 * @file ThreadsTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_THREADSTABLE_HPP
#define PUREPHONE_THREADSTABLE_HPP

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ThreadsTableRow{
    uint32_t ID;
    uint32_t date;
    uint32_t msgCount;
    uint32_t msgRead;
    uint32_t contactID;
    UTF8    snippet;
    SMSType type;
};

class ThreadsTable : public Table<ThreadsTableRow> {
public:

    ThreadsTable();
    ~ThreadsTable();

    bool Add(ThreadsTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(ThreadsTableRow entry) override final;
    ThreadsTableRow GetByID(uint32_t id) override final;
    std::vector<ThreadsTableRow> GetLimitOffset(uint32_t offset,uint32_t limit);

    uint32_t GetCount() override final;

    static const char* GetDBName(){
        return dbName;
    }
private:

    const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS threads("
            "_id INTEGER PRIMARY KEY,"
            "date INTEGER,"
            "msg_count INTEGER,"
            "read INTEGER,"
            "contact_id INTEGER,"
            "snippet TEXT NOT NULL,"
            "last_dir INTEGER"
            ");";

    const char* createTableThreadsCounterQuery =
            "CREATE TABLE IF NOT EXISTS threads_count "
            "(_id INTEGER PRIMARY KEY,"
            "count INTEGER );";

    const char* threadsCounterInsertionQuery = "INSERT OR IGNORE INTO threads_count ( _id, count ) VALUES (1,0);";
    const char* threadInsertTriggerQuery = "CREATE TRIGGER IF NOT EXISTS on_thread_insert AFTER INSERT ON threads BEGIN UPDATE threads_count SET count=count+1 WHERE _id=1; END";
    const char* threadRemoveTriggerQuery = "CREATE TRIGGER IF NOT EXISTS on_thread_remove AFTER DELETE ON threads BEGIN UPDATE threads_count SET count=count-1 WHERE _id=1; END";

    static const char* dbName;

};


#endif //PUREPHONE_THREADSTABLE_HPP
