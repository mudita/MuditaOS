#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/CalllogRecord.hpp"

namespace db::query
{

    class CalllogRemove : public Query
    {
      public:
        CalllogRemove(unsigned int id);
        unsigned int id;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class CalllogRemoveResult : public QueryResult
    {
        bool result;

      public:
        CalllogRemoveResult(bool result);
        [[nodiscard]] auto getResults() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
