// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QuerySMSTemplateGetByID.hpp"

namespace db::query
{
    SMSTemplateGetByID::SMSTemplateGetByID(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto SMSTemplateGetByID::debugInfo() const -> std::string
    {
        return "SMSTemplateGetByID";
    }

    SMSTemplateGetByIDResult::SMSTemplateGetByIDResult(SMSTemplateRecord result) : result(std::move(result))
    {}
    auto SMSTemplateGetByIDResult::getResults() const -> SMSTemplateRecord
    {
        return result;
    }
    auto SMSTemplateGetByIDResult::debugInfo() const -> std::string
    {
        return "SMSTemplateGetByIDResult";
    }

} // namespace db::query
