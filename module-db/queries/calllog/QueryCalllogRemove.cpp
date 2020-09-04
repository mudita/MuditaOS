#include "QueryCalllogRemove.hpp"

namespace db::query
{
    CalllogRemove::CalllogRemove(unsigned int id) : Query(Query::Type::Read), id(id)
    {}

    auto CalllogRemove::debugInfo() const -> std::string
    {
        return "CalllogRemove";
    }

    CalllogRemoveResult::CalllogRemoveResult(bool result) : result(result)
    {}
    auto CalllogRemoveResult::getResults() const -> bool
    {
        return result;
    }
    auto CalllogRemoveResult::debugInfo() const -> std::string
    {
        return "CalllogRemoveResult";
    }

} // namespace db::query
