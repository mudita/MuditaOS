// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/AlarmsRecord.hpp"
#include "Common/Query.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>
#include <string>

namespace db::query::alarms
{
    /// Query for getting alarm reminder for further processing
    class SelectTurnedOn : public Query
    {
      public:
        SelectTurnedOn();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    /// Result of SelectTurnedOn query
    class SelectTurnedOnResult : public QueryResult
    {
        std::vector<AlarmsRecord> records;

      public:
        explicit SelectTurnedOnResult(std::vector<AlarmsRecord> records);
        [[nodiscard]] auto getResult() -> const std::vector<AlarmsRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarms
