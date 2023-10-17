// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/CalllogRecord.hpp"

namespace db::query
{

    class CalllogGetByContactID : public Query
    {
      public:
        CalllogGetByContactID(unsigned int id);
        unsigned int contactId;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class CalllogGetByContactIDResult : public QueryResult
    {
        std::vector<CalllogRecord> result;

      public:
        CalllogGetByContactIDResult(std::vector<CalllogRecord> result);
        [[nodiscard]] auto getResults() const -> std::vector<CalllogRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
