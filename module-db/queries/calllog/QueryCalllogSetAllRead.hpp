// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <string>

namespace db::query::calllog
{
    class SetAllRead : public Query
    {
      public:
        SetAllRead();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SetAllReadResult : public QueryResult
    {
      public:
        explicit SetAllReadResult(bool ret);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

}; // namespace db::query::calllog
