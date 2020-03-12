
/*
 * @file SMSTable.hpp
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

struct SMSTableRow
{
    uint32_t ID;
    uint32_t threadID;
    uint32_t contactID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8 body;
    bool isRead;
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
    virtual ~SMSTable();

    bool Create() override final;
    bool Add(SMSTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(SMSTableFields field, const char *str) override final;
    bool Update(SMSTableRow entry) override final;
    SMSTableRow GetByID(uint32_t id) override final;
    std::vector<SMSTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSTableRow> GetLimitOffsetByField(uint32_t offset,
                                                   uint32_t limit,
                                                   SMSTableFields field,
                                                   const char *str) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS sms("
                                   "_id INTEGER PRIMARY KEY,"
                                   "thread_id INTEGER,"
                                   "contact_id INTEGER,"
                                   "date INTEGER,"
                                   "date_send INTEGER,"
                                   "error_code INTEGER,"
                                   "body TEXT NOT_NULL,"
                                   "read INTEGER,"
                                   "type INTEGER,"
                                   "FOREIGN KEY(thread_id) REFERENCES threads(_id) ON DELETE CASCADE );";
};
