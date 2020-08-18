#include "QueryContactAdd.hpp"
#include "ContactRecord.hpp"

#include <string>

using namespace db::query;

ContactAdd::ContactAdd(const ContactRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

ContactAddResult::ContactAddResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactAdd::debugInfo() const -> std::string
{
    return "ContactAdd";
}

[[nodiscard]] auto ContactAddResult::debugInfo() const -> std::string
{
    return "ContactAddResult";
}
