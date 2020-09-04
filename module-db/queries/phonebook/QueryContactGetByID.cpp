#include "QueryContactGetByID.hpp"

#include <string>

using namespace db::query;

ContactGetByID::ContactGetByID(unsigned int id) : Query(Query::Type::Read), id(id)
{}

ContactGetByIDResult::ContactGetByIDResult(const ContactRecord &record) : record(std::move(record))
{}

[[nodiscard]] auto ContactGetByID::debugInfo() const -> std::string
{
    return "ContactGetByID";
}

[[nodiscard]] auto ContactGetByIDResult::debugInfo() const -> std::string
{
    return "ContactGetByIDResult";
}
