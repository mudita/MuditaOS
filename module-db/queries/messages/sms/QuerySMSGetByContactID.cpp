// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetByContactID.hpp"

namespace db::query
{
    SMSGetByContactID::SMSGetByContactID(unsigned int contactId) : Query(Query::Type::Read), contactId(contactId)
    {}

    auto SMSGetByContactID::debugInfo() const -> std::string
    {
        return "SMSGetByContactID";
    }

    SMSGetByContactIDResult::SMSGetByContactIDResult(std::vector<SMSRecord> result) : result(std::move(result))
    {}
    auto SMSGetByContactIDResult::getResults() const -> std::vector<SMSRecord>
    {
        return result;
    }
    auto SMSGetByContactIDResult::debugInfo() const -> std::string
    {
        return "SMSGetByContactIDResult";
    }

} // namespace db::query
