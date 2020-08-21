#pragma once

#include "SMSTemplateRecord.hpp"
#include <Tables/SMSTemplateTable.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{
    /// implements search for SMS by text
    class SMSTemplateGet : public Query
    {
      public:
        unsigned int offset;
        unsigned int limit;
        SMSTemplateGet(unsigned int offset, unsigned int limit);

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

}; // namespace db::query
