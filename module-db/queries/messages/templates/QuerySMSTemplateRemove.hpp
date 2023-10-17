// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSTemplateRecord.hpp"

namespace db::query
{

    class SMSTemplateRemove : public Query
    {
      public:
        SMSTemplateRemove(unsigned int id);
        unsigned int id;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateRemoveResult : public QueryResult
    {
        bool result;

      public:
        SMSTemplateRemoveResult(bool result);
        [[nodiscard]] auto getResults() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
