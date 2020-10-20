// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactAdd.hpp"
#include "ContactRecord.hpp"

#include <string>

using namespace db::query;

ContactAdd::ContactAdd(const ContactRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

ContactAddResult::ContactAddResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactAdd::debugInfo() const -> std::string
{
    return "ContactAdd";
}

[[nodiscard]] auto ContactAddResult::debugInfo() const -> std::string
{
    return "ContactAddResult";
}
