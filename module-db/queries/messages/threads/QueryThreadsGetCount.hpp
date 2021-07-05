// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <Common/Common.hpp>

namespace db::query
{
    class ThreadGetCount : public Query
    {
        EntryState state;

      public:
        explicit ThreadGetCount(EntryState state);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        [[nodiscard]] auto getState() const noexcept -> EntryState;
    };

    class ThreadGetCountResult : public QueryResult
    {
        EntryState state;
        unsigned count;

      public:
        ThreadGetCountResult(EntryState state, unsigned count);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        [[nodiscard]] auto getState() const noexcept -> EntryState;
        [[nodiscard]] auto getCount() const noexcept -> unsigned;
    };
} // namespace db::query
