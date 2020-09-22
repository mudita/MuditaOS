#include "QuerySMSUpdate.hpp"

using namespace std::literals::string_literals;

namespace db::query
{
    SMSUpdate::SMSUpdate(const SMSRecord &_record) : Query(Query::Type::Update), record{_record}
    {}

    std::string SMSUpdate::debugInfo() const
    {
        return "SMSUpdate"s;
    }

    SMSUpdateResult::SMSUpdateResult(bool _result) : result{_result}
    {}

    std::string SMSUpdateResult::debugInfo() const
    {
        return "SMSUpdateResult"s;
    }
} // namespace db::query
