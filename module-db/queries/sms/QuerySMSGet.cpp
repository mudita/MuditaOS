#include "QuerySMSGet.hpp"

#include <string>

using namespace db::query;

SMSGet::SMSGet(std::size_t limit, std::size_t offset) : RecordQuery(offset, limit)
{}

SMSGetResult::SMSGetResult(const std::vector<SMSRecord> &records) : RecordQueryResult(records)
{}

[[nodiscard]] auto SMSGet::debugInfo() const -> std::string
{
    return "SMSGet";
}

[[nodiscard]] auto SMSGetResult::debugInfo() const -> std::string
{
    return "SMSGetResult";
}
