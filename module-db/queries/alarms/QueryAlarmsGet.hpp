// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "module-db/Interface/AlarmsRecord.hpp"
#include "Common/Query.hpp"
#include <string>

namespace db::query::alarms
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
        AlarmsRecord record;

      public:
        explicit GetResult(const AlarmsRecord &record);
        [[nodiscard]] auto getResult() const -> AlarmsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarms
