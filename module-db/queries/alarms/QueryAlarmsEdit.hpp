// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Interface/AlarmsRecord.hpp"
#include "Common/Query.hpp"
#include <string>

namespace db::query::alarms
{
    class Edit : public Query
    {
        AlarmsRecord record;

      public:
        explicit Edit(const AlarmsRecord &record);

        [[nodiscard]] auto getRecord() const -> AlarmsRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class EditResult : public QueryResult
    {
        bool ret = true;

      public:
        explicit EditResult(bool ret);
        [[nodiscard]] auto succeed() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarms
