
/*
 * @file ThreadsTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ThreadsTableRow
{
    uint32_t ID = 0;
    uint32_t date;
    uint32_t msgCount;
    uint32_t msgRead;
    uint32_t contactID;
    UTF8 snippet;
    SMSType type;
};

enum class ThreadsTableFields
{
    Date,
    MsgCount,
    MsgRead,
    ContactID,
    Type
};

class ThreadsTable : public Table<ThreadsTableRow, ThreadsTableFields>
{
  public:
    ThreadsTable(Database *db);
    virtual ~ThreadsTable();

    bool Create() override final;
    bool Add(ThreadsTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(ThreadsTableRow entry) override final;
    ThreadsTableRow GetByID(uint32_t id) override final;
    std::vector<ThreadsTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<ThreadsTableRow> GetLimitOffsetByField(uint32_t offset,
                                                       uint32_t limit,
                                                       ThreadsTableFields field,
                                                       const char *str) override final;

    uint32_t GetCount() override final;
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;
    ThreadsTableRow getByContact(uint32_t contact_id);

    /// returns: { maximum_query_depth, vector {requested amount of data which match} }
    std::pair<uint32_t, std::vector<ThreadsTableRow>> getBySMSQuery(std::string text, uint32_t offset, uint32_t limit);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS threads("
                                   "_id INTEGER PRIMARY KEY,"
                                   "date INTEGER,"
                                   "msg_count INTEGER,"
                                   "read INTEGER,"
                                   "contact_id INTEGER,"
                                   "snippet TEXT NOT NULL,"
                                   "last_dir INTEGER"
                                   ");";

    const char *createTableThreadsCounterQuery = "CREATE TABLE IF NOT EXISTS threads_count "
                                                 "(_id INTEGER PRIMARY KEY,"
                                                 "count INTEGER );";

    const char *threadsCounterInsertionQuery = "INSERT OR IGNORE INTO threads_count ( _id, count ) VALUES (1,0);";
    const char *threadInsertTriggerQuery     = "CREATE TRIGGER IF NOT EXISTS on_thread_insert AFTER INSERT ON threads "
                                           "BEGIN UPDATE threads_count SET count=count+1 WHERE _id=1; END";
    const char *threadRemoveTriggerQuery = "CREATE TRIGGER IF NOT EXISTS on_thread_remove AFTER DELETE ON threads "
                                           "BEGIN UPDATE threads_count SET count=count-1 WHERE _id=1; END";
};
