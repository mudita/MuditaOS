#include "QueryContactGet.hpp"

#include <string>

using namespace db::query;

ContactGet::ContactGet(const std::string &filter) : TextFilter(filter)
{}

ContactGet::ContactGet(std::size_t limit, std::size_t offset, const std::string &filter)
    : RecordQuery(limit, offset), TextFilter(filter)
{}

ContactGetResult::ContactGetResult(const std::vector<ContactRecord> &records) : RecordQueryResult(records)
{}

ContactGetSize::ContactGetSize(const std::string &filter) : TextFilter(filter)
{}

[[nodiscard]] auto ContactGet::debugInfo() const -> std::string
{
    return "ContactGet";
}

[[nodiscard]] auto ContactGetResult::debugInfo() const -> std::string
{
    return "ContactGetResult";
}

[[nodiscard]] auto ContactGetSize::debugInfo() const -> std::string
{
    return "ContactGetSize";
}
