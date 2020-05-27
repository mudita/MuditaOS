#include "Record.hpp"
#include <Common/Query.hpp>
#include <log/log.hpp>

namespace db
{

    std::unique_ptr<QueryResult> Interface::runQuery(const Query *query)
    {
        LOG_DEBUG("Query not handled! debugInfo: %s", query ? query->debugInfo().c_str() : "empty");
        return nullptr;
    }
} // namespace db
