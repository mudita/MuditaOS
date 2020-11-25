// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class EditICS : public Query
    {
        EventsRecord record;

      public:
        EditICS(EventsRecord record);

        [[nodiscard]] auto getRecord() const -> EventsRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class EditICSResult : public QueryResult
    {
      public:
        explicit EditICSResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;

        bool ret;
    };

} // namespace db::query::events
