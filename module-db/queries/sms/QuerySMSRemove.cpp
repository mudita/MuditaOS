#include "QuerySMSRemove.hpp"

namespace db::query
{
    SMSRemove::SMSRemove(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto SMSRemove::debugInfo() const -> std::string
    {
        return "SMSRemove";
    }

    SMSRemoveResult::SMSRemoveResult(bool result) : result(result)
    {}
    auto SMSRemoveResult::getResults() const -> bool
    {
        return result;
    }
    auto SMSRemoveResult::debugInfo() const -> std::string
    {
        return "SMSRemoveResult";
    }

} // namespace db::query
