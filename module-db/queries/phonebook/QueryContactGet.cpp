// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryContactGet.hpp"

#include <string>

using namespace db::query;

ContactGet::ContactGet(const std::string &filter, const std::uint32_t &groupFilter, const std::uint32_t &displayMode)
    : TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGet::ContactGet(std::size_t limit,
                       std::size_t offset,
                       const std::string &filter,
                       std::uint32_t groupFilter,
                       std::uint32_t displayMode)
    : RecordQuery(limit, offset), TextFilter(filter), ContactGroupFilter(groupFilter), ContactDisplayMode(displayMode)
{}

ContactGetWithTotalCount::ContactGetWithTotalCount(std::size_t limit,
                                                   std::size_t offset,
                                                   const std::string &filter,
                                                   std::uint32_t groupFilter,
                                                   std::uint32_t displayMode)
    : ContactGet(limit, offset, filter, groupFilter, displayMode)
{}

ContactGetResult::ContactGetResult(const std::vector<ContactRecord> &records) : RecordQueryResult(records)
{}

ContactGetResultWithTotalCount::ContactGetResultWithTotalCount(const std::vector<ContactRecord> &records,
                                                               std::size_t allLength)
    : ContactGetResult(records), allLength(allLength)
{}

auto ContactGetResultWithTotalCount::getAllLength() const -> std::size_t
{
    return allLength;
}

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

ContactGetLetterMapResult::ContactGetLetterMapResult(ContactsMapData &LetterMap) : LetterMapResult(LetterMap)
{}

[[nodiscard]] auto ContactGet::debugInfo() const -> std::string
{
    return "ContactGet";
}

[[nodiscard]] auto ContactGetWithTotalCount::debugInfo() const -> std::string
{
    return "ContactGetWithTotalCount";
}

[[nodiscard]] auto ContactGetResult::debugInfo() const -> std::string
{
    return "ContactGetResult";
}

[[nodiscard]] auto ContactGetResultWithTotalCount::debugInfo() const -> std::string
{
    return "ContactGetResultWithTotalCount";
}

[[nodiscard]] auto ContactGetSize::debugInfo() const -> std::string
{
    return "ContactGetSize";
}
