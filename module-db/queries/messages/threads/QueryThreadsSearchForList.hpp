// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/ThreadRecord.hpp>
#include <Interface/ContactRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{
    /// implements search for SMS by text
    class ThreadsSearchForList : public Query
    {
      public:
        std::string textToSearch;
        unsigned int offset;
        unsigned int limit;
        ThreadsSearchForList(std::string textToSearch, unsigned int offset, unsigned int limit);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadsSearchResultForList : public QueryResult
    {
        std::vector<ThreadRecord> results;
        std::vector<ContactRecord> contacts;
        unsigned int count = 0;

      public:
        ThreadsSearchResultForList(std::vector<ThreadRecord> result,
                                   std::vector<ContactRecord> contacts,
                                   unsigned int count);
        [[nodiscard]] auto getCount() const -> unsigned int;
        [[nodiscard]] auto getResults() const -> std::vector<ThreadRecord>;
        [[nodiscard]] auto getContacts() const -> std::vector<ContactRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
