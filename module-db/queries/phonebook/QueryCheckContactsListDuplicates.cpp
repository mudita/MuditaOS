// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCheckContactsListDuplicates.hpp"
#include <string>

using namespace db::query;

CheckContactsListDuplicates::CheckContactsListDuplicates(std::vector<ContactRecord> contacts)
    : Query(Query::Type::Read), contacts(std::move(contacts))
{}

std::vector<ContactRecord> &CheckContactsListDuplicates::getContactsList()
{
    return contacts;
}

[[nodiscard]] auto CheckContactsListDuplicates::debugInfo() const -> std::string
{
    return "CheckContactsListDuplicates";
}

CheckContactsListDuplicatesResult::CheckContactsListDuplicatesResult(
    std::pair<std::vector<ContactRecord>, std::vector<ContactRecord>> result)
    : unique(std::move(result.first)), duplicates(std::move(result.second))
{}

std::vector<ContactRecord> &CheckContactsListDuplicatesResult::getUnique()
{
    return unique;
}

std::vector<ContactRecord> &CheckContactsListDuplicatesResult::getDuplicates()
{
    return duplicates;
}

[[nodiscard]] auto CheckContactsListDuplicatesResult::debugInfo() const -> std::string
{
    return "CheckContactsListDuplicatesResult";
}
