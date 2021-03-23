// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"
#include <utf8/UTF8.hpp>

struct QuotesTableRow : public Record
{
    UTF8 text;
    time_t lastUsageTimestamp = 0;
};

enum class QuotesTableFields
{
};

class QuotesTable : public Table<QuotesTableRow, QuotesTableFields>
{
  public:
    QuotesTable(Database *db);
    virtual ~QuotesTable();

    bool create() override final;
    bool add(QuotesTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(QuotesTableFields field, const char *str) override final;
    bool update(QuotesTableRow entry) override final;
    QuotesTableRow getById(uint32_t id) override final;
    std::vector<QuotesTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<QuotesTableRow> getLimitOffsetByField(uint32_t offset,
                                                      uint32_t limit,
                                                      QuotesTableFields field,
                                                      const char *str) override final;

    uint32_t count() override final;
    uint32_t count(EntryState state);
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
};
