// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/SMSRecord.hpp>

#include <string>

namespace db::query
{
    class SMSGet : public RecordQuery
    {
      public:
        SMSGet(std::size_t limit, std::size_t offset);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetWithTotalCount : public SMSGet
    {
      public:
        SMSGetWithTotalCount(std::size_t limit, std::size_t offset);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetResult : public RecordQueryResult<SMSRecord>
    {
      public:
        SMSGetResult(std::vector<SMSRecord> records);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetResultWithTotalCount : public SMSGetResult
    {
      public:
        SMSGetResultWithTotalCount(std::vector<SMSRecord> records, std::size_t totalCount);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getTotalCount() const -> std::size_t;

      private:
        std::size_t totalCount;
    };

}; // namespace db::query
