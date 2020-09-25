#include "QuerySMSTemplateGetCount.hpp"

namespace db::query
{
    SMSTemplateGetCount::SMSTemplateGetCount() : Query(Query::Type::Read)
    {}

    auto SMSTemplateGetCount::debugInfo() const -> std::string
    {
        return "SMSTemplateGetCount";
    }

    SMSTemplateGetCountResult::SMSTemplateGetCountResult(uint32_t result) : result(result)
    {}
    auto SMSTemplateGetCountResult::getResults() const -> uint32_t
    {
        return result;
    }
    auto SMSTemplateGetCountResult::debugInfo() const -> std::string
    {
        return "SMSTemplateGetCountResult";
    }

} // namespace db::query
