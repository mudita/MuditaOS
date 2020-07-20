#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSTemplateRecord.hpp"

namespace db::query
{
    /// implements search for SMS by type
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
