// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        unsigned int threadId;
        unsigned int offset;
        unsigned int limit;

        SMSGetByThreadID(unsigned int id, unsigned int offset = 0, unsigned int limit = 0);
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
