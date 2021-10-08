// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/SMSTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{
    class SMSGetCount : public Query
    {
      public:
        SMSGetCount();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSGetCountResult : public QueryResult
    {
        uint32_t result;

      public:
        SMSGetCountResult(uint32_t result);
        [[nodiscard]] auto getResults() const -> uint32_t;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
