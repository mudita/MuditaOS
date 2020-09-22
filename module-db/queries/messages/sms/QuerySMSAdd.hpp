#pragma once

#include <cstdint>

#include <Common/Query.hpp>
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    class SMSAdd : public Query
    {
      public:
        explicit SMSAdd(const SMSRecord &_record);

        [[nodiscard]] std::string debugInfo() const override;
        SMSRecord record;
    };

    class SMSAddResult : public QueryResult
    {
      public:
        explicit SMSAddResult(const SMSRecord &_record, bool _result);

        [[nodiscard]] std::string debugInfo() const override;
        SMSRecord record;
        bool result;
    };
} // namespace db::query
