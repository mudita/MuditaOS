#include "QueryContactRemove.hpp"

#include <string>

using namespace db::query;

ContactRemove::ContactRemove(unsigned int id) : Query(Query::Type::Read), id(id)
{}

ContactRemoveResult::ContactRemoveResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactRemove::debugInfo() const -> std::string
{
    return "ContactRemove";
}

[[nodiscard]] auto ContactRemoveResult::debugInfo() const -> std::string
{
    return "ContactRemoveResult";
}
