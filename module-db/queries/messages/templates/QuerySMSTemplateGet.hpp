// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SMSTemplateRecord.hpp"
#include <Tables/SMSTemplateTable.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{

    class SMSTemplateGet : public Query
    {
      public:
        unsigned int limit;
        unsigned int offset;
        SMSTemplateGet(unsigned int limit, unsigned int offset);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetWithTotalCount : public SMSTemplateGet
    {
      public:
        SMSTemplateGetWithTotalCount(std::size_t limit, std::size_t offset);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetResult : public QueryResult
    {
        std::vector<SMSTemplateRecord> results;

      public:
        SMSTemplateGetResult(std::vector<SMSTemplateRecord> result_rows);
        [[nodiscard]] auto getResults() const -> std::vector<SMSTemplateRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetResultWithTotalCount : public SMSTemplateGetResult
    {
      public:
        SMSTemplateGetResultWithTotalCount(std::vector<SMSTemplateRecord> records, std::size_t totalTemplatesCount);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        [[nodiscard]] auto getTotalTemplatesCount() const noexcept -> std::size_t;

      private:
        std::size_t totalTemplatesCount;
    };

}; // namespace db::query
