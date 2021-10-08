// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetCount.hpp"

namespace db::query
{
    SMSGetCount::SMSGetCount() : Query(Query::Type::Read)
    {}

    auto SMSGetCount::debugInfo() const -> std::string
    {
        return "SMSGetCount";
    }

    SMSGetCountResult::SMSGetCountResult(uint32_t result) : result(result)
    {}
    auto SMSGetCountResult::getResults() const -> uint32_t
    {
        return result;
    }
    auto SMSGetCountResult::debugInfo() const -> std::string
    {
        return "SMSGetCountResult";
    }

} // namespace db::query
