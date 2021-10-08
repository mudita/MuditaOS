// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactRemove.hpp"

#include <string>

using namespace db::query;

ContactRemove::ContactRemove(unsigned int id) : Query(Query::Type::Read), id(id)
{}

ContactRemoveResult::ContactRemoveResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactRemove::debugInfo() const -> std::string
{
    return "ContactRemove";
}

[[nodiscard]] auto ContactRemoveResult::debugInfo() const -> std::string
{
    return "ContactRemoveResult";
}
