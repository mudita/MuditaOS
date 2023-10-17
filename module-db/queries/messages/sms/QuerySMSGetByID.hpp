// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetByID : public Query
    {
      public:
        SMSGetByID(unsigned int id);
        unsigned int id;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetByIDResult : public QueryResult
    {
        SMSRecord result;

      public:
        SMSGetByIDResult(SMSRecord result);
        [[nodiscard]] auto getResults() const -> SMSRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
