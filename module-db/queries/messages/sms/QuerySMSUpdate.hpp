// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
