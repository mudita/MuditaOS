#pragma once

#include <string>

#include <Common/Query.hpp>
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    class SMSUpdate : public Query
    {
      public:
        explicit SMSUpdate(const SMSRecord &_record);

        [[nodiscard]] std::string debugInfo() const override;
        SMSRecord record;
    };

    class SMSUpdateResult : public QueryResult
    {
      public:
        explicit SMSUpdateResult(bool _result);

        std::string debugInfo() const override;
        bool result;
    };
} // namespace db::query
