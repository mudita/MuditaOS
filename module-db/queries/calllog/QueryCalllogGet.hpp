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
        CalllogGetResult(const std::vector<CalllogRecord> &records);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
