// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"

#include <cstdint>
#include <filesystem>

#include <utf8/UTF8.hpp>

#include "module-db/Databases/NotesDB.hpp"
#include "../Common/Common.hpp"

struct NotesRecord : public Record
{
    std::uint32_t date;
    UTF8 snippet;
    std::filesystem::path path;
};

enum class NotesRecordField
{
    Date,
    Snippet,
    Path
};

class NotesRecordInterface : public RecordInterface<NotesRecord, NotesRecordField>
{
  public:
    explicit NotesRecordInterface(NotesDB *notesDb);

    bool Add(const NotesRecord &rec) override;
    bool RemoveByID(uint32_t id) override;
    bool RemoveByField(NotesRecordField field, const char *str) override;
    bool Update(const NotesRecord &rec) override;
    NotesRecord GetByID(uint32_t id) override;
    uint32_t GetCount() override;
    std::unique_ptr<std::vector<NotesRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override;
    std::unique_ptr<std::vector<NotesRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                    uint32_t limit,
                                                                    NotesRecordField field,
                                                                    const char *str) override;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    std::vector<NotesRecord> getNotes(uint32_t offset, uint32_t limit) const;

    std::unique_ptr<db::QueryResult> getForListQuery(const std::shared_ptr<db::Query> &query);

    NotesDB *notesDB;
};
