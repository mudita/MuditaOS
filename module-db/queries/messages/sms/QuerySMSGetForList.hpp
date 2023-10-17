// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        unsigned int numberID = 0;

        SMSGetForList(unsigned int id, unsigned int offset = 0, unsigned int limit = 0, unsigned int numberID = 0);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetForListResult : public QueryResult
    {
        std::vector<SMSRecord> result;
        unsigned int count = 0;
        SMSRecord draft;
        utils::PhoneNumber::View number;

      public:
        SMSGetForListResult(std::vector<SMSRecord> result,
                            unsigned int count,
                            SMSRecord draft,
                            const utils::PhoneNumber::View &number);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto getCount() const -> unsigned int;
        [[nodiscard]] auto getDraft() const -> SMSRecord;
        [[nodiscard]] auto getNumber() const -> utils::PhoneNumber::View;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
