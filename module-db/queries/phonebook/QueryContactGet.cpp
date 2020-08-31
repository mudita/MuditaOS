#include "QueryContactGet.hpp"

#include <string>

using namespace db::query;

ContactGet::ContactGet(const std::string &filter, const std::uint32_t &groupFilter)
    : TextFilter(filter), ContactGroupFilter(groupFilter)
{}

ContactGet::ContactGet(std::size_t limit,
                       std::size_t offset,
                       const std::string &filter,
                       const std::uint32_t &groupFilter)
    : RecordQuery(limit, offset), TextFilter(filter), ContactGroupFilter(groupFilter)
{}

ContactGetResult::ContactGetResult(const std::vector<ContactRecord> &records) : RecordQueryResult(records)
{}

ContactGetSize::ContactGetSize(const std::string &filter, const std::uint32_t &groupFilter)
    : TextFilter(filter), ContactGroupFilter(groupFilter)
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
