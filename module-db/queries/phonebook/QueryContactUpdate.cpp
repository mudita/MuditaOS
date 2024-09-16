// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryContactUpdate.hpp"
#include "ContactRecord.hpp"

#include <string>

using namespace db::query;

ContactUpdate::ContactUpdate(const ContactRecord &rec) : Query(Query::Type::Update), rec(std::move(rec))
{}

ContactUpdateResult::ContactUpdateResult(bool result,
                                         std::uint32_t id,
                                         const std::vector<utils::PhoneNumber::View> &duplicates)
    : result(result), duplicates(duplicates), id(id)
{}

[[nodiscard]] auto ContactUpdate::debugInfo() const -> std::string
{
    return "ContactUpdate";
}

[[nodiscard]] auto ContactUpdateResult::debugInfo() const -> std::string
{
    return "ContactUpdateResult";
}
