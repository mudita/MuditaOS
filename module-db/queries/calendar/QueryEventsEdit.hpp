// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class Edit : public Query
    {
        EventsRecord record;

      public:
        Edit(EventsRecord record);

        [[nodiscard]] auto getRecord() const -> EventsRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class EditResult : public QueryResult
    {
      public:
        EditResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

} // namespace db::query::events
