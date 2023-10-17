// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/AlarmEventRecord.hpp"

#include <Common/Query.hpp>

#include <string>

namespace db::query::alarmEvents
{
    class GetInRange : public Query
    {
      public:
        GetInRange(uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t offset = 0;
        const uint32_t limit  = 0;
    };

    class GetInRangeResult : public QueryResult
    {
        const std::vector<AlarmEventRecord> records;
        const std::uint32_t count = 0;

      public:
        explicit GetInRangeResult(std::vector<AlarmEventRecord> records, std::uint32_t count);
        [[nodiscard]] auto getResult() const -> std::pair<std::vector<AlarmEventRecord>, std::uint32_t>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
