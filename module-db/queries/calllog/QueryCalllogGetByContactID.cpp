// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryCalllogGetByContactID.hpp"

namespace db::query
{
    CalllogGetByContactID::CalllogGetByContactID(unsigned int contactId)
        : Query(Query::Type::Read), contactId(contactId)
    {}

    auto CalllogGetByContactID::debugInfo() const -> std::string
    {
        return "CalllogGetByContactID";
    }

    CalllogGetByContactIDResult::CalllogGetByContactIDResult(std::vector<CalllogRecord> result)
        : result(std::move(result))
    {}
    auto CalllogGetByContactIDResult::getResults() const -> std::vector<CalllogRecord>
    {
        return result;
    }
    auto CalllogGetByContactIDResult::debugInfo() const -> std::string
    {
        return "CalllogGetByContactIDResult";
    }

} // namespace db::query
