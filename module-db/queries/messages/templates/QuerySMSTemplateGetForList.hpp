// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <string>
#include <Interface/SMSTemplateRecord.hpp>

namespace db::query
{

    class SMSTemplateGetForList : public Query
    {
      public:
        unsigned int offset = 0;
        unsigned int limit  = 0;

        SMSTemplateGetForList(unsigned int offset = 0, unsigned int limit = 0);
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetForListResult : public QueryResult
    {
        std::vector<SMSTemplateRecord> result;
        unsigned int count = 0;

      public:
        SMSTemplateGetForListResult(std::vector<SMSTemplateRecord> result, unsigned int count);

        [[nodiscard]] auto getResults() const -> std::vector<SMSTemplateRecord>;
        [[nodiscard]] auto getCount() const -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
