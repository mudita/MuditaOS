// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "Common/Common.hpp"

#include <utf8/UTF8.hpp>

struct ThreadsTableRow : public Record
{
    uint32_t date           = 0;
    uint32_t msgCount       = 0;
    uint32_t unreadMsgCount = 0;
    uint32_t contactID      = DB_ID_NONE;
    uint32_t numberID       = DB_ID_NONE;
    UTF8 snippet;
    SMSType type = SMSType::UNKNOWN;
};

enum class ThreadsTableFields
{
    Date,
    MsgCount,
    MsgRead,
    ContactID,
    NumberID,
    Type
};

class ThreadsTable : public Table<ThreadsTableRow, ThreadsTableFields>
{
  public:
    ThreadsTable(Database *db);
    virtual ~ThreadsTable() = default;

    bool create() override final;
    bool add(ThreadsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool update(ThreadsTableRow entry) override final;
    ThreadsTableRow getById(uint32_t id) override final;
    std::vector<ThreadsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<ThreadsTableRow> getLimitOffsetByField(uint32_t offset,
                                                       uint32_t limit,
                                                       ThreadsTableFields field,
                                                       const char *str) override final;

    uint32_t count() override final;
    uint32_t count(EntryState state);
    uint32_t countByFieldId(const char *field, uint32_t id) override final;
    ThreadsTableRow getByContact(uint32_t contact_id);

    /// returns: { maximum_query_depth, vector {requested amount of data which match} }
    std::pair<uint32_t, std::vector<ThreadsTableRow>> getBySMSQuery(std::string text, uint32_t offset, uint32_t limit);
};
