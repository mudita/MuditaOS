// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetByThreadID.hpp"

namespace db::query
{
    SMSGetByThreadID::SMSGetByThreadID(unsigned int threadId, unsigned int limit, unsigned int offset)
        : Query(Query::Type::Read), threadId(threadId), limit(limit), offset(offset)
    {}

    auto SMSGetByThreadID::debugInfo() const -> std::string
    {
        return "SMSGetByThreadID";
    }

    SMSGetByThreadIDWithTotalCount::SMSGetByThreadIDWithTotalCount(unsigned int threadId,
                                                                   std::size_t limit,
                                                                   std::size_t offset)
        : SMSGetByThreadID(threadId, limit, offset)
    {}

    [[nodiscard]] auto SMSGetByThreadIDWithTotalCount::debugInfo() const -> std::string
    {
        return "SMSGetByThreadIDWithTotalCount";
    }

    SMSGetByThreadIDResult::SMSGetByThreadIDResult(std::vector<SMSRecord> result) : result(std::move(result))
    {}

    auto SMSGetByThreadIDResult::getResults() const -> std::vector<SMSRecord>
    {
        return result;
    }

    auto SMSGetByThreadIDResult::debugInfo() const -> std::string
    {
        return "SMSGetByThreadIDResult";
    }

    SMSGetByThreadIDResultWithTotalCount::SMSGetByThreadIDResultWithTotalCount(std::vector<SMSRecord> records,
                                                                               std::size_t totalCount)
        : SMSGetByThreadIDResult(std::move(records)), totalCount(totalCount)
    {}

    auto SMSGetByThreadIDResultWithTotalCount::getTotalCount() const -> std::size_t
    {
        return totalCount;
    }

    auto SMSGetByThreadIDResultWithTotalCount::debugInfo() const -> std::string
    {
        return "SMSGetByThreadIDResultWithTotalCount";
    }

} // namespace db::query
