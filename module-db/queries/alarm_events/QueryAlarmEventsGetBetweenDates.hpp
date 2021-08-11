// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        GetBetweenDates(TimePoint start, TimePoint end, uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const TimePoint start = TIME_POINT_INVALID;
        const TimePoint end   = TIME_POINT_INVALID;
        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetBetweenDatesResult : public QueryResult
    {
        const std::vector<AlarmEventRecord> records;

      public:
        explicit GetBetweenDatesResult(std::vector<AlarmEventRecord> records);
        [[nodiscard]] auto getResult() const -> std::vector<AlarmEventRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
