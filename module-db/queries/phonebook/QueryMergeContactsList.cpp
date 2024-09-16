// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryMergeContactsList.hpp"
#include <string>

using namespace db::query;

MergeContactsList::MergeContactsList(std::vector<ContactRecord> contacts)
    : Query(Query::Type::Read), contacts(std::move(contacts))
{}

std::vector<ContactRecord> &MergeContactsList::getContactsList()
{
    return contacts;
}

MergeContactsListResult::MergeContactsListResult(const std::vector<std::pair<db::Query::Type, uint32_t>> &addedContacts)
    : addedContacts(addedContacts)
{}

std::vector<std::pair<db::Query::Type, uint32_t>> &MergeContactsListResult::getResult()
{
    return addedContacts;
}

[[nodiscard]] auto MergeContactsList::debugInfo() const -> std::string
{
    return "MergeContactsList";
}

[[nodiscard]] auto MergeContactsListResult::debugInfo() const -> std::string
{
    return "MergeContactsListResult";
}
