#include "QuerySMSTemplateAdd.hpp"
#include "SMSTemplateRecord.hpp"

#include <string>

using namespace db::query;

SMSTemplateAdd::SMSTemplateAdd(const SMSTemplateRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

SMSTemplateAddResult::SMSTemplateAddResult(bool result) : result(result)
{}

[[nodiscard]] auto SMSTemplateAdd::debugInfo() const -> std::string
{
    return "SMSTemplateAdd";
}

[[nodiscard]] auto SMSTemplateAddResult::debugInfo() const -> std::string
{
    return "SMSTemplateAddResult";
}
