// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Record.hpp"
#include <Common/Query.hpp>
#include <log/log.hpp>

namespace db
{

    std::unique_ptr<QueryResult> Interface::runQuery(std::shared_ptr<db::Query> query)
    {
        LOG_DEBUG("Query not handled! debugInfo: %s", query ? query->debugInfo().c_str() : "empty");
        return nullptr;
    }
} // namespace db
