// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <utility>
#include "QuerySMSTemplateGet.hpp"

namespace db::query
{
    SMSTemplateGet::SMSTemplateGet(unsigned int limit, unsigned int offset)
        : Query(Query::Type::Read), limit(limit), offset(offset)
    {}

    auto SMSTemplateGet::debugInfo() const -> std::string
    {
        return "SMSTemplateGet";
    }

    SMSTemplateGetWithTotalCount::SMSTemplateGetWithTotalCount(std::size_t limit, std::size_t offset)
        : SMSTemplateGet(limit, offset)
    {}

    [[nodiscard]] auto SMSTemplateGetWithTotalCount::debugInfo() const -> std::string
    {
        return "SMSTemplateGetWithTotalCount";
    }

    SMSTemplateGetResult::SMSTemplateGetResult(std::vector<SMSTemplateRecord> result) : results(std::move(result))
    {}

    auto SMSTemplateGetResult::getResults() const -> std::vector<SMSTemplateRecord>
    {
        return results;
    }

    [[nodiscard]] auto SMSTemplateGetResult::debugInfo() const -> std::string
    {
        return "SMSTemplateGetResults";
    }

    SMSTemplateGetResultWithTotalCount::SMSTemplateGetResultWithTotalCount(std::vector<SMSTemplateRecord> records,
                                                                           std::size_t totalTemplatesCount)
        : SMSTemplateGetResult(std::move(records)), totalTemplatesCount(totalTemplatesCount)
    {}

    auto SMSTemplateGetResultWithTotalCount::getTotalTemplatesCount() const noexcept -> std::size_t
    {
        return totalTemplatesCount;
    }

    [[nodiscard]] auto SMSTemplateGetResultWithTotalCount::debugInfo() const -> std::string
    {
        return "SMSTemplateGetResultWithTotalCount";
    }
} // namespace db::query
