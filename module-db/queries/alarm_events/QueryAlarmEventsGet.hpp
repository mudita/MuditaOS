// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

#include <string>

namespace db::query::alarmEvents
{
    class Get : public Query
    {
      public:
        const uint32_t id;

        explicit Get(uint32_t id);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetResult : public QueryResult
    {
        const AlarmEventRecord record;

      public:
        explicit GetResult(const AlarmEventRecord &record);
        [[nodiscard]] auto getResult() const -> AlarmEventRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
