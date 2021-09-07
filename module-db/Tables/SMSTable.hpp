// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
    std::vector<SMSTableRow> getByText(std::string text, uint32_t threadId);
    std::vector<SMSTableRow> getByThreadId(uint32_t threadId, uint32_t offset, uint32_t limit);
    std::vector<SMSTableRow> getByThreadIdWithoutDraftWithEmptyInput(uint32_t threadId,
                                                                     uint32_t offset,
                                                                     uint32_t limit);
    uint32_t countWithoutDraftsByThreadId(uint32_t threadId);
    SMSTableRow getDraftByThreadId(uint32_t threadId);

    std::pair<uint32_t, std::vector<SMSTableRow>> getManyByType(SMSType type, uint32_t offset, uint32_t limit);
};
