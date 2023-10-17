// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Tables/ThreadsTable.hpp>
#include <utility>
#include "QueryThreadsGetForList.hpp"

namespace db::query
{
    ThreadsGetForList::ThreadsGetForList(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}
    auto ThreadsGetForList::debugInfo() const -> std::string
    {
        return "SMSThreadsGetForList";
    }

    ThreadsGetForListResults::ThreadsGetForListResults(std::vector<ThreadRecord> results,
                                                       std::vector<ContactRecord> contacts,
                                                       std::vector<utils::PhoneNumber::View> numbers,
                                                       unsigned int count)
        : results(std::move(results)), contacts(std::move(contacts)), numbers(std::move(numbers)), count(count)
    {}
    auto ThreadsGetForListResults::getResults() const -> std::vector<ThreadRecord>
    {
        return results;
    }
    auto ThreadsGetForListResults::getContacts() const -> std::vector<ContactRecord>
    {
        return contacts;
    }
    auto ThreadsGetForListResults::getNumbers() const -> std::vector<utils::PhoneNumber::View>
    {
        return numbers;
    }
    auto ThreadsGetForListResults::getCount() const -> unsigned int
    {
        return count;
    }
    auto ThreadsGetForListResults::debugInfo() const -> std::string
    {
        return "SMSThreadsGetForListResults";
    }

} // namespace db::query
