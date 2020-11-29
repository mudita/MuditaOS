// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/AlarmsRecord.hpp"
#include <Common/Query.hpp>
#include <string>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace db::query::alarms
{
    /// Query for getting alarm  reminder is first in filtered time range
    class SelectFirstUpcoming : public Query
    {
      public:
        SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        TimePoint filter_from;
        TimePoint filter_till;
    };

    /// Result of SelectFirstUpcoming query
    class SelectFirstUpcomingResult : public QueryResult
    {
        std::unique_ptr<std::vector<AlarmsRecord>> records;

      public:
        SelectFirstUpcomingResult(std::unique_ptr<std::vector<AlarmsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<AlarmsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarms
