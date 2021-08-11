// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/AlarmEventRecord.hpp"
#include "Common/Query.hpp"
#include <string>

namespace db::query::alarmEvents
{
    class GetLimited : public Query
    {
      public:
        explicit GetLimited(uint32_t offset, uint32_t limit);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        uint32_t offset = 0;
        uint32_t limit  = 0;
    };

    class GetLimitedResult : public QueryResult
    {
        std::vector<AlarmEventRecord> records;

      public:
        explicit GetLimitedResult(std::vector<AlarmEventRecord> records);
        [[nodiscard]] auto getResult() -> std::vector<AlarmEventRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
