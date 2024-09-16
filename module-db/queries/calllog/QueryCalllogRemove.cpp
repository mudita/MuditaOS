// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryCalllogRemove.hpp"

namespace db::query
{
    CalllogRemove::CalllogRemove(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto CalllogRemove::debugInfo() const -> std::string
    {
        return "CalllogRemove";
    }

    CalllogRemoveResult::CalllogRemoveResult(bool result) : result(result)
    {}
    auto CalllogRemoveResult::getResults() const -> bool
    {
        return result;
    }
    auto CalllogRemoveResult::debugInfo() const -> std::string
    {
        return "CalllogRemoveResult";
    }

} // namespace db::query
