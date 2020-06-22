/*
 * @file CalllogTable.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Table
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

enum class CallType
{
    CT_NONE      = 0x00,
    CT_INCOMING  = 0x01,
    CT_OUTGOING  = 0x02,
    CT_MISSED    = 0x03,
    CT_VOICEMAIL = 0x04,
    CT_REJECTED  = 0x05,
    CT_BLOCKED   = 0x06,
    CT_ANSW_EXT  = 0x07,
};

enum class PresentationType
{
    PR_UNKNOWN    = 0x00,
    PR_ALLOWED    = 0x01,
    PR_PAYPHONE   = 0x02,
    PR_RESTRICTED = 0x03,
};

struct CalllogTableRow : public Record
{
    UTF8 number;
    UTF8 e164number;
    PresentationType presentation = PresentationType::PR_UNKNOWN;
    time_t date                   = 0;
    time_t duration               = 0;
    CallType type                 = CallType::CT_NONE;
    UTF8 name;
    UTF8 contactId;
    bool isRead = true;
};

enum class CalllogTableFields
{
    DATE,
    TYPE,
};

/*
 *
 */
class CalllogTable : public Table<CalllogTableRow, CalllogTableFields>
{
  public:
    CalllogTable(Database *db);
    virtual ~CalllogTable();

    bool Create() override final;
    bool Add(CalllogTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(CalllogTableFields field, const char *str) override final;
    bool Update(CalllogTableRow entry) override final;
    CalllogTableRow GetByID(uint32_t id) override final;
    std::vector<CalllogTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<CalllogTableRow> GetLimitOffsetByField(uint32_t offset,
                                                       uint32_t limit,
                                                       CalllogTableFields field,
                                                       const char *str) override final;

    uint32_t GetCount() override final;
    uint32_t GetCount(EntryState state);
    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;
    bool SetAllRead();

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS calls("
                                   "_id INTEGER PRIMARY KEY,"
                                   "number TEXT DEFAULT '',"
                                   "e164number TEXT DEFAULT '',"
                                   "presentation INTEGER DEFAULT 0,"
                                   "date INTEGER DEFAULT 0,"
                                   "duration INTEGER DEFAULT 0,"
                                   "type INTEGER DEFAULT 0,"
                                   "name TEXT DEFAULT '',"
                                   "contactId TEXT DEFAULT '',"
                                   "isRead INTEGER DEFAULT 1)";
};
