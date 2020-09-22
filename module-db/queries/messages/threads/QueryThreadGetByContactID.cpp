#include "QueryThreadGetByContactID.hpp"

using namespace std::string_literals;

namespace db::query
{
    ThreadGetByContactID::ThreadGetByContactID(unsigned int _id) : Query{Query::Type::Read}, id{_id}
    {}

    std::string ThreadGetByContactID::debugInfo() const
    {
        return "ThreadGetByContactID"s;
    }

    ThreadGetByContactIDResult::ThreadGetByContactIDResult(const std::optional<ThreadRecord> &_record) : record{_record}
    {}

    std::optional<ThreadRecord> ThreadGetByContactIDResult::getRecord() const noexcept
    {
        return record;
    }

    std::string ThreadGetByContactIDResult::debugInfo() const
    {
        return "ThreadGetByContactIDResult"s;
    }
} // namespace db::query
