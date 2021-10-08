// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactGetByID.hpp"

#include <string>

using namespace db::query;

ContactGetByID::ContactGetByID(unsigned int id, bool withTemporary)
    : Query(Query::Type::Read), id(id), withTemporary(withTemporary)
{}

ContactGetByIDResult::ContactGetByIDResult(const ContactRecord &record) : record(std::move(record))
{}

[[nodiscard]] auto ContactGetByID::debugInfo() const -> std::string
{
    return "ContactGetByID";
}

[[nodiscard]] auto ContactGetByIDResult::debugInfo() const -> std::string
{
    return "ContactGetByIDResult";
}
