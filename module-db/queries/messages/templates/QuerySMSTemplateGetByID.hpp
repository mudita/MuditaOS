// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSTemplateRecord.hpp"

namespace db::query
{

    class SMSTemplateGetByID : public Query
    {
      public:
        SMSTemplateGetByID(unsigned int id);
        unsigned int id;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSTemplateGetByIDResult : public QueryResult
    {
        SMSTemplateRecord result;

      public:
        SMSTemplateGetByIDResult(SMSTemplateRecord result);
        [[nodiscard]] auto getResults() const -> SMSTemplateRecord;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
