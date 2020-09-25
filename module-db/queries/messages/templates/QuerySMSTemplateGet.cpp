#include <utility>
#include "QuerySMSTemplateGet.hpp"

namespace db::query
{
    SMSTemplateGet::SMSTemplateGet(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto SMSTemplateGet::debugInfo() const -> std::string
    {
        return "SMSTemplateGet";
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
} // namespace db::query
