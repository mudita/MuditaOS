// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/SMSTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSTemplateRecord.hpp"

namespace db::query
{
    class SMSTemplateGetCount : public Query
    {
      public:
        SMSTemplateGetCount();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetCountResult : public QueryResult
    {
        uint32_t result;

      public:
        SMSTemplateGetCountResult(uint32_t result);
        [[nodiscard]] auto getResults() const -> uint32_t;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
