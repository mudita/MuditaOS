// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/CalllogRecord.hpp>

#include <string>

namespace db::query
{
    class CalllogGet : public RecordQuery
    {
      public:
        CalllogGet(std::size_t limit, std::size_t offset);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class CalllogGetResult : public RecordQueryResult<CalllogRecord>
    {
      public:
        CalllogGetResult(std::vector<CalllogRecord> &&records, unsigned int dbRecordsCount);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        [[nodiscard]] auto getTotalCount() const noexcept -> unsigned int;

      private:
        unsigned int dbRecordsCount;
    };

}; // namespace db::query
