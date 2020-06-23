#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct NotificationsTableRow : public Record
{
    uint32_t key   = 0;
    uint32_t value = 0;
};

enum class NotificationsTableFields
{
    key
};

/*
 *
 */
class NotificationsTable : public Table<NotificationsTableRow, NotificationsTableFields>
{
  public:
    NotificationsTable(Database *db);
    virtual ~NotificationsTable();

    bool Create() override final;
    bool Add(NotificationsTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(NotificationsTableFields field, const char *str) override final;
    bool Update(NotificationsTableRow entry) override final;
    NotificationsTableRow GetByID(uint32_t id) override final;
    NotificationsTableRow GetByKey(uint32_t key);
    uint32_t GetCount() override final;
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;
    std::vector<NotificationsTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<NotificationsTableRow> GetLimitOffsetByField(uint32_t offset,
                                                             uint32_t limit,
                                                             NotificationsTableFields field,
                                                             const char *str) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS notifications("
                                   "_id INTEGER PRIMARY KEY,"
                                   "key INTEGER UNIQUE DEFAULT 0,"
                                   "value INTEGER DEFAULT 0)";
};
