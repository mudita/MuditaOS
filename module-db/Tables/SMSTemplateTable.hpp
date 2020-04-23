#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"
#include <utf8/UTF8.hpp>

struct SMSTemplateTableRow : public Record
{
    UTF8 text;
    time_t lastUsageTimestamp;
};

enum class SMSTemplateTableFields
{
};

class SMSTemplateTable : public Table<SMSTemplateTableRow, SMSTemplateTableFields>
{
  public:
    SMSTemplateTable(Database *db);
    virtual ~SMSTemplateTable();

    bool Create() override final;
    bool Add(SMSTemplateTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSTemplateTableFields field, const char *str) override final;
    bool Update(SMSTemplateTableRow entry) override final;
    SMSTemplateTableRow GetByID(uint32_t id) override final;
    std::vector<SMSTemplateTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSTemplateTableRow> GetLimitOffsetByField(uint32_t offset,
                                                           uint32_t limit,
                                                           SMSTemplateTableFields field,
                                                           const char *str) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS templates("
                                   "_id INTEGER PRIMARY KEY,"
                                   "text TEXT,"
                                   "lastUsageTimestamp INTEGER)";
};
