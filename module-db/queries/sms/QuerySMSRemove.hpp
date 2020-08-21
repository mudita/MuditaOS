#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{
    /// implements search for SMS by type
    class SMSRemove : public Query
    {
      public:
        SMSRemove(unsigned int id);
        unsigned int id;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSRemoveResult : public QueryResult
    {
        bool result;

      public:
        SMSRemoveResult(bool result);
        [[nodiscard]] auto getResults() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
