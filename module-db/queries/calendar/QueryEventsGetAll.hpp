// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{
    class GetAll : public Query
    {
      public:
        GetAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetAllResult : public QueryResult
    {
        std::vector<EventsRecord> records;

      public:
        GetAllResult(std::vector<EventsRecord> records);
        [[nodiscard]] auto getResult() -> std::vector<EventsRecord>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
