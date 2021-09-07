// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

#include <string>

namespace db::query::alarmEvents
{
    class GetBetweenDates : public Query
    {
      public:
        GetBetweenDates(TimePoint start, TimePoint end, std::uint32_t offset, std::uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const TimePoint start = TIME_POINT_INVALID;
        const TimePoint end   = TIME_POINT_INVALID;
        const std::uint32_t offset = 0;
        const std::uint32_t limit  = 0;
    };

    class GetBetweenDatesResult : public QueryResult
    {
        const std::vector<AlarmEventRecord> records;
        const std::uint32_t count = 0;

      public:
        explicit GetBetweenDatesResult(std::vector<AlarmEventRecord> records, std::uint32_t count);
        [[nodiscard]] auto getResult() const -> std::pair<std::vector<AlarmEventRecord>, std::uint32_t>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
