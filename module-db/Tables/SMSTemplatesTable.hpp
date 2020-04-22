#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"
#include <utf8/UTF8.hpp>

struct SMSTemplatesTableRow : public Record
{
    UTF8 text;
    uint32_t lastUsageTimestamp;
};

enum class SMSTemplatesTableFields
{
};

class SMSTemplatesTable : public Table<SMSTemplatesTableRow, SMSTemplatesTableFields>
{
  public:
    SMSTemplatesTable(Database *db);
    virtual ~SMSTemplatesTable();

    bool Create() override final;
    bool Add(SMSTemplatesTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSTemplatesTableFields field, const char *str) override final;
    bool Update(SMSTemplatesTableRow entry) override final;
    SMSTemplatesTableRow GetByID(uint32_t id) override final;
    std::vector<SMSTemplatesTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSTemplatesTableRow> GetLimitOffsetByField(uint32_t offset,
                                                            uint32_t limit,
                                                            SMSTemplatesTableFields field,
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
