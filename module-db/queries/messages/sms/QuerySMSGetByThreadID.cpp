#include "QuerySMSGetByThreadID.hpp"

namespace db::query
{
    SMSGetByThreadID::SMSGetByThreadID(unsigned int threadId, unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), threadId(threadId), offset(offset), limit(limit)
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
