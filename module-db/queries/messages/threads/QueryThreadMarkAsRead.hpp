// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <string>

namespace db::query
{
    class MarkAsRead : public Query
    {
      public:
        enum class Read : bool
        {
            True,
            False
        };
        const Read read;
        const uint32_t id;
        MarkAsRead(uint32_t id, Read read = Read::True);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class MarkAsReadResult : public QueryResult
    {
        bool ret;

      public:
        MarkAsReadResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
