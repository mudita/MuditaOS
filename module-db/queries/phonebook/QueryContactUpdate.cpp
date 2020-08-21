#include "QueryContactUpdate.hpp"
#include "ContactRecord.hpp"

#include <string>

using namespace db::query;

ContactUpdate::ContactUpdate(const ContactRecord &rec) : Query(Query::Type::Read), rec(std::move(rec))
{}

ContactUpdateResult::ContactUpdateResult(bool result) : result(result)
{}

[[nodiscard]] auto ContactUpdate::debugInfo() const -> std::string
{
    return "ContactUpdate";
}

[[nodiscard]] auto ContactUpdateResult::debugInfo() const -> std::string
{
    return "ContactUpdateResult";
}
