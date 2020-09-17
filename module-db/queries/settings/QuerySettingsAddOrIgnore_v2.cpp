#include "QuerySettingsAddOrIgnore_v2.hpp"

namespace db::query::settings
{
    AddOrIgnoreQuery::AddOrIgnoreQuery(const SettingsRecord_v2 &record) : Query{Query::Type::Update}, record{record}
    {}

    auto AddOrIgnoreQuery::debugInfo() const -> std::string
    {
        return "Update";
    }

    auto AddOrIgnoreQuery::getRecord() const noexcept -> const SettingsRecord_v2 &
    {
        return record;
    }

    AddOrIgnoreResult::AddOrIgnoreResult(const bool &value) : value{value}
    {}

    auto AddOrIgnoreResult::getValue() const -> bool
    {
        return value;
    }

    auto AddOrIgnoreResult::debugInfo() const -> std::string
    {
        return "AddOrIgnoreResult";
    }
} // namespace db::query::settings
