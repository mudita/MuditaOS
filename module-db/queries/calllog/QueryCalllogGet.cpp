#include "QueryCalllogGet.hpp"

#include <string>

using namespace db::query;

CalllogGet::CalllogGet(std::size_t limit, std::size_t offset) : RecordQuery(limit, offset)
{}

CalllogGetResult::CalllogGetResult(const std::vector<CalllogRecord> &records) : RecordQueryResult(records)
{}

[[nodiscard]] auto CalllogGet::debugInfo() const -> std::string
{
    return "CalllogGet";
}

[[nodiscard]] auto CalllogGetResult::debugInfo() const -> std::string
{
    return "CalllogGetResult";
}
