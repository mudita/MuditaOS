#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct SMSTableRow : public Record
{
    uint32_t threadID;
    uint32_t contactID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8 body;
    SMSType type;
};

enum class SMSTableFields
{
    Date,
    ThreadID,
    ContactID
};

class SMSTable : public Table<SMSTableRow, SMSTableFields>
{
  public:
    SMSTable(Database *db);
    virtual ~SMSTable() = default;

    bool create() override final;
    bool add(SMSTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(SMSTableFields field, const char *str) override final;
    bool update(SMSTableRow entry) override final;
    SMSTableRow getById(uint32_t id) override final;
    std::vector<SMSTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSTableRow> getLimitOffsetByField(uint32_t offset,
                                                   uint32_t limit,
                                                   SMSTableFields field,
                                                   const char *str) override final;

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    std::vector<SMSTableRow> getByContactId(uint32_t contactId);
    std::vector<SMSTableRow> getByText(std::string text);
    std::vector<SMSTableRow> getByThreadId(uint32_t threadId);

    std::pair<uint32_t, std::vector<SMSTableRow>> getManyByType(SMSType type, uint32_t offset, uint32_t limit);

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS sms("
                                   "_id INTEGER PRIMARY KEY,"
                                   "thread_id INTEGER,"
                                   "contact_id INTEGER,"
                                   "date INTEGER,"
                                   "date_send INTEGER,"
                                   "error_code INTEGER,"
                                   "body TEXT NOT_NULL,"
                                   "type INTEGER,"
                                   "FOREIGN KEY(thread_id) REFERENCES threads(_id) ON DELETE CASCADE );";
};
