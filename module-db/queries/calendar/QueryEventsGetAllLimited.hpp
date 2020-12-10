// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/EventsRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class GetAllLimited : public Query
    {
      public:
        GetAllLimited(const uint32_t &offset, const uint32_t &limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        uint32_t offset = 0;
        uint32_t limit  = 0;
    };

    class GetAllLimitedResult : public QueryResult
    {
        std::vector<EventsRecord> records;
        uint32_t recordsCount;

      public:
        GetAllLimitedResult(std::vector<EventsRecord> records, uint32_t count);
        [[nodiscard]] auto getResult() -> std::vector<EventsRecord>;
        [[nodiscard]] auto getCountResult() const noexcept -> uint32_t;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
