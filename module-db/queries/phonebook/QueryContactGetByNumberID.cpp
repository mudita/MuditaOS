// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactGetByNumberID.hpp"

using namespace db::query;

ContactGetByNumberID::ContactGetByNumberID(std::uint32_t numberID) : Query(Query::Type::Read), numberID{numberID}
{}

[[nodiscard]] auto ContactGetByNumberID::debugInfo() const -> std::string
{
    return "ContactGetByNumberID";
}

ContactGetByNumberIDResult::ContactGetByNumberIDResult(const ContactRecord &record) : record(std::move(record))
{}

[[nodiscard]] auto ContactGetByNumberIDResult::debugInfo() const -> std::string
{
    return "ContactGetByNumberIDResult";
}
