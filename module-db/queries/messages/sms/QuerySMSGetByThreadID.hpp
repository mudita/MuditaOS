// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        unsigned int limit;
        unsigned int offset;

        explicit SMSGetByThreadID(unsigned int id, unsigned int limit = 0, unsigned int offset = 0);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetByThreadIDWithTotalCount : public SMSGetByThreadID
    {
      public:
        SMSGetByThreadIDWithTotalCount(unsigned int threadId, std::size_t limit, std::size_t offset);
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

    class SMSGetByThreadIDResultWithTotalCount : public SMSGetByThreadIDResult
    {
      public:
        SMSGetByThreadIDResultWithTotalCount(std::vector<SMSRecord> records, std::size_t totalCount);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getTotalCount() const -> std::size_t;

      private:
        std::size_t totalCount;
    };

} // namespace db::query
