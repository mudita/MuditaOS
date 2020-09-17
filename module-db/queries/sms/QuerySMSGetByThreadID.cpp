#include "QuerySMSGetByThreadID.hpp"

namespace db::query
{
    SMSGetByThreadID::SMSGetByThreadID(unsigned int threadId) : Query(Query::Type::Read), threadId(threadId)
    {}

    auto SMSGetByThreadID::debugInfo() const -> std::string
    {
        return "SMSGetByThreadID";
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

} // namespace db::query
