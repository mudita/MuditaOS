// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetForList.hpp"

#include <utility>

namespace db::query
{
    SMSGetForList::SMSGetForList(unsigned int threadId, unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), threadId(threadId), offset(offset), limit(limit)
    {}

    auto SMSGetForList::debugInfo() const -> std::string
    {
        return "SMSGetForList";
    }

    SMSGetForListResult::SMSGetForListResult(std::vector<SMSRecord> result, unsigned int count, SMSRecord draft)
        : result(std::move(result)), count(count), draft(std::move(draft))
    {}
    auto SMSGetForListResult::getResults() const -> std::vector<SMSRecord>
    {
        return result;
    }
    auto SMSGetForListResult::getCount() const -> unsigned int
    {
        return count;
    }
    auto SMSGetForListResult::getDraft() const -> SMSRecord
    {
        return draft;
    }
    auto SMSGetForListResult::debugInfo() const -> std::string
    {
        return "SMSGetForListResult";
    }
} // namespace db::query
