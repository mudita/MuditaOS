// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetByID.hpp"

namespace db::query
{
    SMSGetByID::SMSGetByID(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto SMSGetByID::debugInfo() const -> std::string
    {
        return "SMSGetByID";
    }

    SMSGetByIDResult::SMSGetByIDResult(SMSRecord result) : result(std::move(result))
    {}
    auto SMSGetByIDResult::getResults() const -> SMSRecord
    {
        return result;
    }
    auto SMSGetByIDResult::debugInfo() const -> std::string
    {
        return "SMSGetByIDResult";
    }

} // namespace db::query
