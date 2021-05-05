// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct NotesTableRow
{
    std::uint32_t ID = DB_ID_NONE;
    std::uint32_t date;
    UTF8 snippet;
};

enum class NotesTableFields
{
    Date,
    Snippet,
};

class NotesTable : public Table<NotesTableRow, NotesTableFields>
{
  public:
    explicit NotesTable(Database *db);

    bool create() override;
    bool add(NotesTableRow entry) override;
    bool removeAll() override;
    bool removeById(std::uint32_t id) override;
    bool removeByField(NotesTableFields field, const char *str) override;
    bool update(NotesTableRow entry) override;
    NotesTableRow getById(std::uint32_t id) override;
    std::vector<NotesTableRow> getLimitOffset(std::uint32_t offset, std::uint32_t limit) override;
    std::vector<NotesTableRow> getLimitOffsetByField(std::uint32_t offset,
                                                     std::uint32_t limit,
                                                     NotesTableFields field,
                                                     const char *str) override;
    std::pair<std::vector<NotesTableRow>, int> getByText(const std::string &text,
                                                         unsigned int offset,
                                                         unsigned int limit);

    std::uint32_t count() override;
    std::uint32_t countByText();
    std::uint32_t countByFieldId(const char *field, std::uint32_t id) override;
};
