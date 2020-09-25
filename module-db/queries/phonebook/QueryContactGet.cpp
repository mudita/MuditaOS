#include "QueryContactGet.hpp"

#include <string>

using namespace db::query;

ContactGet::ContactGet(const std::string &filter, const std::uint32_t &groupFilter, const std::uint32_t &displayMode)
    : TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGet::ContactGet(std::size_t limit,
                       std::size_t offset,
                       const std::string &filter,
                       const std::uint32_t &groupFilter,
                       const std::uint32_t &displayMode)
    : RecordQuery(limit, offset), TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGetResult::ContactGetResult(const std::vector<ContactRecord> &records) : RecordQueryResult(records)
{}

ContactGetSize::ContactGetSize(const std::string &filter,
                               const std::uint32_t &groupFilter,
                               const std::uint32_t &displayMode)
    : TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGetLetterMap::ContactGetLetterMap(const std::string &filter,
                                         const std::uint32_t &groupFilter,
                                         const std::uint32_t &displayMode)
    : TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGetLetterMap::ContactGetLetterMap(std::size_t limit,
                                         std::size_t offset,
                                         const std::string &filter,
                                         const std::uint32_t &groupFilter,
                                         const std::uint32_t &displayMode)
    : RecordQuery(limit, offset), TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGetLetterMapResult ::ContactGetLetterMapResult(ContactsMapData &LetterMap) : LetterMapResult(LetterMap)
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
