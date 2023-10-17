// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vector>
#include "QuerySMSSearchByType.hpp"
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    SMSSearchByType::SMSSearchByType(SMSType type_to_search, unsigned int starting_position, unsigned int depth)
        : Query(Query::Type::Read), type(type_to_search), starting_postion(starting_position), depth(depth)
    {}

    auto SMSSearchByType::debugInfo() const -> std::string
    {
        return "SMSSearchByType";
    }

    SMSSearchByTypeResult::SMSSearchByTypeResult(unsigned int results_max_depth, std::vector<SMSRecord> result_rows)
        : results(result_rows), results_max_depth(results_max_depth)
    {}
    auto SMSSearchByTypeResult::getResults() const -> std::vector<SMSRecord>
    {
        return results;
    }
    auto SMSSearchByTypeResult::debugInfo() const -> std::string
    {
        return "SMSSearchByTypeResult";
    }
    auto SMSSearchByTypeResult::getMaxDepth() const -> unsigned int
    {
        return results_max_depth;
    }

} // namespace db::query
