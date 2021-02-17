// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RecordQuery.hpp"

#include <Common/Query.hpp>

#include <string>
#include <utility>

using namespace db::query;

RecordQuery::RecordQuery() noexcept : Query(Query::Type::Read)
{}

RecordQuery::RecordQuery(std::size_t limit, std::size_t offset) noexcept
    : Query(Query::Type::Read), limit(limit), offset(offset)
{}

RecordsSizeQuery::RecordsSizeQuery() noexcept : Query(Query::Type::Read)
{}

[[nodiscard]] std::pair<std::size_t, std::size_t> RecordQuery::getLimitOffset() const noexcept
{
    return std::make_pair(limit, offset);
}

[[nodiscard]] std::size_t RecordQuery::getLimit() const noexcept
{
    return limit;
}

[[nodiscard]] std::size_t RecordQuery::getOffset() const noexcept
{
    return offset;
}

RecordsSizeQueryResult::RecordsSizeQueryResult(std::size_t size) noexcept : size(size)
{}

[[nodiscard]] std::size_t RecordsSizeQueryResult::getSize() const noexcept
{
    return size;
}

[[nodiscard]] auto RecordQuery::debugInfo() const -> std::string
{
    return "RecordQuery";
}

[[nodiscard]] auto RecordsSizeQuery::debugInfo() const -> std::string
{
    return "RecordsSizeQuery";
}

[[nodiscard]] auto RecordsSizeQueryResult::debugInfo() const -> std::string
{
    return "RecordsSizeQueryResult";
}
