// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>

#include <string>

namespace db::query::alarmEvents
{

    class ToggleAll : public Query
    {
      public:
        explicit ToggleAll(bool toggle);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool toggle;
    };

    class ToggleAllResult : public QueryResult
    {
        const bool ret = true;

      public:
        explicit ToggleAllResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarmEvents
