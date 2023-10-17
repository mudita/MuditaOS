// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSTemplateGetForList.hpp"

#include <utility>

namespace db::query
{
    SMSTemplateGetForList::SMSTemplateGetForList(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto SMSTemplateGetForList::debugInfo() const -> std::string
    {
        return "SMSTemplateGetForList";
    }

    SMSTemplateGetForListResult::SMSTemplateGetForListResult(std::vector<SMSTemplateRecord> result, unsigned int count)
        : result(std::move(result)), count(count)
    {}
    auto SMSTemplateGetForListResult::getResults() const -> std::vector<SMSTemplateRecord>
    {
        return result;
    }
    auto SMSTemplateGetForListResult::getCount() const -> unsigned int
    {
        return count;
    }
    auto SMSTemplateGetForListResult::debugInfo() const -> std::string
    {
        return "SMSTemplateGetForListResult";
    }
} // namespace db::query
