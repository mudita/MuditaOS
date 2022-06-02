// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
    time_t lastUsageTimestamp = 0;
};

enum class SMSTemplateTableFields
{
};

class SMSTemplateTable : public Table<SMSTemplateTableRow, SMSTemplateTableFields>
{
  public:
    SMSTemplateTable(Database *db);
    virtual ~SMSTemplateTable();

    bool create() override final;
    bool add(SMSTemplateTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(SMSTemplateTableFields field, const char *str) override final;
    bool update(SMSTemplateTableRow entry) override final;
    SMSTemplateTableRow getById(uint32_t id) override final;
    std::vector<SMSTemplateTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<SMSTemplateTableRow> getLimitOffsetByField(uint32_t offset,
                                                           uint32_t limit,
                                                           SMSTemplateTableFields field,
                                                           const char *str) override final;

    uint32_t count() override final;
    uint32_t count(EntryState state);
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    uint32_t getLastId();
};
