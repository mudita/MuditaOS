// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryThreadsSearchForList.hpp"

#include <utility>

namespace db::query
{
    ThreadsSearchForList::ThreadsSearchForList(std::string textToSearch, unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), textToSearch(std::move(textToSearch)), offset(offset), limit(limit)
    {}

    auto ThreadsSearchForList::debugInfo() const -> std::string
    {
        return "SMSSearch";
    }

    ThreadsSearchResultForList::ThreadsSearchResultForList(std::vector<ThreadRecord> results,
                                                           std::vector<ContactRecord> contacts,
                                                           unsigned int count)
        : results(std::move(results)), contacts(std::move(contacts)), count(count)
    {}

    auto ThreadsSearchResultForList::getCount() const -> unsigned int
    {
        return count;
    }

    auto ThreadsSearchResultForList::getResults() const -> std::vector<ThreadRecord>
    {
        return results;
    }

    auto ThreadsSearchResultForList::getContacts() const -> std::vector<ContactRecord>
    {
        return contacts;
    }

    [[nodiscard]] auto ThreadsSearchResultForList::debugInfo() const -> std::string
    {
        return "SMSSearchResult";
    }
} // namespace db::query
