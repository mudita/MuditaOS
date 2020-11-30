// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/EventsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::events
{

    class RemoveICS : public Query
    {
      public:
        RemoveICS(const std::string &UID);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        std::string UID;
    };

    class RemoveICSResult : public QueryResult
    {
        bool ret;

      public:
        explicit RemoveICSResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::events
