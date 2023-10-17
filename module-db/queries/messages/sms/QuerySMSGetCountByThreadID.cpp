// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetCountByThreadID.hpp"

namespace db::query
{
    SMSGetCountByThreadID::SMSGetCountByThreadID(unsigned int threadId) : Query(Query::Type::Read), threadId(threadId)
    {}

    auto SMSGetCountByThreadID::debugInfo() const -> std::string
    {
        return "SMSGetCountByThreadID";
    }

    SMSGetCountByThreadIDResult::SMSGetCountByThreadIDResult(uint32_t result) : result(result)
    {}
    auto SMSGetCountByThreadIDResult::getResults() const -> uint32_t
    {
        return result;
    }
    auto SMSGetCountByThreadIDResult::debugInfo() const -> std::string
    {
        return "SMSGetCountByThreadIDResult";
    }

} // namespace db::query
