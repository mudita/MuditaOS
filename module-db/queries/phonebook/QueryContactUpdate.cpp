// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactUpdate.hpp"
#include "ContactRecord.hpp"

#include <string>

using namespace db::query;

ContactUpdate::ContactUpdate(const ContactRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

ContactUpdateResult::ContactUpdateResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactUpdate::debugInfo() const -> std::string
{
    return "ContactUpdate";
}

[[nodiscard]] auto ContactUpdateResult::debugInfo() const -> std::string
{
    return "ContactUpdateResult";
}
