// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/SMSTemplateRecord.hpp>

#include <string>

namespace db::query
{

    class SMSTemplateAdd : public Query
    {
      public:
        explicit SMSTemplateAdd(const SMSTemplateRecord &rec);
        SMSTemplateRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateAddResult : public QueryResult
    {
      public:
        SMSTemplateAddResult(bool result, unsigned int id);
        auto getResult() -> bool
        {
            return result;
        }
        [[nodiscard]] auto getID() const noexcept -> unsigned int
        {
            return id;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
        unsigned int id;
    };

}; // namespace db::query
