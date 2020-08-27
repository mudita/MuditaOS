#include "QuerySettingsUpdate_v2.hpp"

namespace db::query::settings
{
    UpdateQuery::UpdateQuery(const SettingsRecord_v2 &record) : Query{Query::Type::Update}, record{record}
    {}

    auto UpdateQuery::debugInfo() const -> std::string
    {
        return "Update";
    }

    auto UpdateQuery::getRecord() const -> const SettingsRecord_v2 &
    {
        return record;
    }

    UpdateResult::UpdateResult(const bool &value) : value{value}
    {}

    auto UpdateResult::getValue() const -> bool
    {
        return value;
    }

    auto UpdateResult::debugInfo() const -> std::string
    {
        return "UpdateResult";
    }
} // namespace db::query::settings
