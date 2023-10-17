// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/SMSTemplateRecord.hpp>

#include <string>

namespace db::query
{

    class SMSTemplateUpdate : public Query
    {
      public:
        SMSTemplateUpdate(const SMSTemplateRecord &rec);
        SMSTemplateRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateUpdateResult : public QueryResult
    {
      public:
        SMSTemplateUpdateResult(bool result);
        auto getResult() -> bool
        {
            return result;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
    };

}; // namespace db::query
