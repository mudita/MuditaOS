// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSRemove.hpp"

namespace db::query
{
    SMSRemove::SMSRemove(unsigned int id) : Query(Query::Type::Delete), id(id)
    {}

    auto SMSRemove::debugInfo() const -> std::string
    {
        return "SMSRemove";
    }

    SMSRemoveResult::SMSRemoveResult(bool result) : result(result)
    {}
    auto SMSRemoveResult::getResults() const -> bool
    {
        return result;
    }
    auto SMSRemoveResult::debugInfo() const -> std::string
    {
        return "SMSRemoveResult";
    }

} // namespace db::query
