// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"

#include <cstdint>
#include <filesystem>

#include <utf8/UTF8.hpp>

#include "module-db/databases/NotesDB.hpp"
#include "../Common/Common.hpp"

struct NotesRecord : public Record
{
    std::uint32_t date;
    UTF8 snippet;
};

enum class NotesRecordField
{
    Date,
    Snippet,
};

class NotesRecordInterface : public RecordInterface<NotesRecord, NotesRecordField>
{
  public:
    explicit NotesRecordInterface(NotesDB *notesDb);

    bool Add(const NotesRecord &rec) override;
    bool RemoveAll() override;
    bool RemoveByID(std::uint32_t id) override;
    bool RemoveByField(NotesRecordField field, const char *str) override;
    bool Update(const NotesRecord &rec) override;
    NotesRecord GetByID(std::uint32_t id) override;
    std::uint32_t GetCount() override;
    std::unique_ptr<std::vector<NotesRecord>> GetLimitOffset(std::uint32_t offset, std::uint32_t limit) override;
    std::unique_ptr<std::vector<NotesRecord>> GetLimitOffsetByField(std::uint32_t offset,
                                                                    std::uint32_t limit,
                                                                    NotesRecordField field,
                                                                    const char *str) override;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    std::vector<NotesRecord> getNotes(std::uint32_t offset, std::uint32_t limit) const;
    std::pair<std::vector<NotesRecord>, unsigned int> getNotesByText(const std::string &text,
                                                                     unsigned int offset,
                                                                     unsigned int limit) const;

    std::unique_ptr<db::QueryResult> getQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getByTextQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> storeQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> removeQuery(const std::shared_ptr<db::Query> &query);

    NotesDB *notesDB;
};
