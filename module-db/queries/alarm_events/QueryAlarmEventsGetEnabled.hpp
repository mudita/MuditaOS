// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/AlarmEventRecord.hpp"

#include <Common/Query.hpp>

#include <string>

namespace db::query::alarmEvents
{
    class GetEnabled : public Query
    {
      public:
        GetEnabled();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetEnabledResult : public QueryResult
    {
        const std::vector<AlarmEventRecord> records;

      public:
        explicit GetEnabledResult(std::vector<AlarmEventRecord> records);
        [[nodiscard]] auto getResult() const -> std::vector<AlarmEventRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
