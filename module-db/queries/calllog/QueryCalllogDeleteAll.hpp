// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <string>

namespace db::query::calllog
{
    class DeleteAll : public Query
    {
      public:
        DeleteAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class DeleteAllResult : public QueryResult
    {
      public:
        explicit DeleteAllResult(bool ret);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const bool ret = true;
    };

}; // namespace db::query::calllog
