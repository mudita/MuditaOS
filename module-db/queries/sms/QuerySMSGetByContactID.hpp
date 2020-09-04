#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetByContactID : public Query
    {
      public:
        SMSGetByContactID(unsigned int id);
        unsigned int contactId;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetByContactIDResult : public QueryResult
    {
        std::vector<SMSRecord> result;

      public:
        SMSGetByContactIDResult(std::vector<SMSRecord> result);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
