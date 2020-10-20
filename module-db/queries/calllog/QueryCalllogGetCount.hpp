// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/CalllogRecord.hpp>

#include <string>

namespace db::query
{
    class CalllogGetCount : public Query
    {
      public:
        CalllogGetCount();
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class CalllogGetCountResult : public QueryResult
    {
      private:
        uint32_t count;

      public:
        CalllogGetCountResult(const uint32_t count);
        [[nodiscard]] auto getCount() const -> uint32_t;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
