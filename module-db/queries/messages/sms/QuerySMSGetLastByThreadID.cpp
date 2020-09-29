#include "QuerySMSGetLastByThreadID.hpp"

using namespace std::string_literals;

namespace db::query
{
    SMSGetLastByThreadID::SMSGetLastByThreadID(unsigned int _threadId) : Query{Query::Type::Read}, threadId{_threadId}
    {}

    std::string SMSGetLastByThreadID::debugInfo() const
    {
        return "SMSGetLastByThreadID"s;
    }

    SMSGetLastByThreadIDResult::SMSGetLastByThreadIDResult(const std::optional<SMSRecord> &_record) : record{_record}
    {}

    std::string SMSGetLastByThreadIDResult::debugInfo() const
    {
        return "SMSGetLastByThreadIDResult"s;
    }
} // namespace db::query
