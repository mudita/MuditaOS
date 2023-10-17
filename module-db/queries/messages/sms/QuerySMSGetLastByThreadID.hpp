// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <module-db/Interface/SMSRecord.hpp>

namespace db::query
{
    class SMSGetLastByThreadID : public Query
    {
      public:
        explicit SMSGetLastByThreadID(unsigned int _threadId);

        [[nodiscard]] std::string debugInfo() const override;

        unsigned int threadId;
    };

    class SMSGetLastByThreadIDResult : public QueryResult
    {
      public:
        explicit SMSGetLastByThreadIDResult(const std::optional<SMSRecord> &_record);

        [[nodiscard]] std::string debugInfo() const override;

        std::optional<SMSRecord> record;
    };
} // namespace db::query
