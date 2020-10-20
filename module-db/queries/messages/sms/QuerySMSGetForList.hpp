// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetForList : public Query
    {
      public:
        unsigned int threadId = DB_ID_NONE;
        unsigned int offset   = 0;
        unsigned int limit    = 0;

        SMSGetForList(unsigned int id, unsigned int offset = 0, unsigned int limit = 0);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetForListResult : public QueryResult
    {
        std::vector<SMSRecord> result;
        unsigned int count = 0;
        SMSRecord draft;

      public:
        SMSGetForListResult(std::vector<SMSRecord> result, unsigned int count, SMSRecord draft);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto getCount() const -> unsigned int;
        [[nodiscard]] auto getDraft() const -> SMSRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
