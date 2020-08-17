#include "QuerySMSGetByText.hpp"

namespace db::query
{
    SMSGetByText::SMSGetByText(std::string text) : Query(Query::Type::Read), text(std::move(text))
    {}

    auto SMSGetByText::debugInfo() const -> std::string
    {
        return "SMSGetByText";
    }

    SMSGetByTextResult::SMSGetByTextResult(std::vector<SMSRecord> result) : result(std::move(result))
    {}
    auto SMSGetByTextResult::getResults() const -> std::vector<SMSRecord>
    {
        return result;
    }
    auto SMSGetByTextResult::debugInfo() const -> std::string
    {
        return "SMSGetByTextResult";
    }

} // namespace db::query
