// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryMultipleContactRemove.hpp"

#include <string>

using namespace db::query;

MultipleContactRemove::MultipleContactRemove(const std::vector<std::uint32_t> &ids)
    : Query(Query::Type::Delete), ids(ids)
{}

MultipleContactRemoveResult::MultipleContactRemoveResult(bool result) : result(result)
{}

[[nodiscard]] auto MultipleContactRemove::debugInfo() const -> std::string
{
    return "MultipleContactRemove";
}

[[nodiscard]] auto MultipleContactRemoveResult::debugInfo() const -> std::string
{
    return "ContactRemoveResult";
}
