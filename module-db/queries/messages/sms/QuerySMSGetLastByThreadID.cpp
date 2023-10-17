// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetLastByThreadID.hpp"

using namespace std::string_literals;

namespace db::query
{
    SMSGetLastByThreadID::SMSGetLastByThreadID(unsigned int _threadId) : Query{Query::Type::Read}, threadId{_threadId}
    {}

    std::string SMSGetLastByThreadID::debugInfo() const
    {
        return "SMSGetLastByThreadID"s;
    }

    SMSGetLastByThreadIDResult::SMSGetLastByThreadIDResult(const std::optional<SMSRecord> &_record) : record{_record}
    {}

    std::string SMSGetLastByThreadIDResult::debugInfo() const
    {
        return "SMSGetLastByThreadIDResult"s;
    }
} // namespace db::query
