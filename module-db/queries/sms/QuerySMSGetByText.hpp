#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetByText : public Query
    {
      public:
        SMSGetByText(std::string text);
        std::string text;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetByTextResult : public QueryResult
    {
        std::vector<SMSRecord> result;

      public:
        SMSGetByTextResult(std::vector<SMSRecord> result);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
