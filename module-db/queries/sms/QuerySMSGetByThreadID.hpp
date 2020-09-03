#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetByThreadID : public Query
    {
      public:
        SMSGetByThreadID(unsigned int id);
        unsigned int threadId;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetByThreadIDResult : public QueryResult
    {
        std::vector<SMSRecord> result;

      public:
        SMSGetByThreadIDResult(std::vector<SMSRecord> result);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
