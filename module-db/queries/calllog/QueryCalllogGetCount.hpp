// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/CalllogRecord.hpp>
#include <Common/Common.hpp>

#include <string>

namespace db::query
{
    class CalllogGetCount : public Query
    {
        EntryState state;

      public:
        explicit CalllogGetCount(EntryState state);
        [[nodiscard]] auto getState() const noexcept -> EntryState;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class CalllogGetCountResult : public QueryResult
    {
        EntryState state;
        unsigned count;

      public:
        CalllogGetCountResult(EntryState state, unsigned count);
        [[nodiscard]] auto getState() const noexcept -> EntryState;
        [[nodiscard]] auto getCount() const noexcept -> unsigned;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
