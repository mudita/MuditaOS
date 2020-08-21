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

    class SMSGetResult : public RecordQueryResult<SMSRecord>
    {
      public:
        SMSGetResult(const std::vector<SMSRecord> &records);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
