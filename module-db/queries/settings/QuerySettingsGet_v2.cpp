// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySettingsGet_v2.hpp"

namespace db::query::settings
{
    SettingsQuery::SettingsQuery(std::string path) : Query{Query::Type::Read}, path{path}
    {}

    auto SettingsQuery::debugInfo() const -> std::string
    {
        return "Get";
    }

    auto SettingsQuery::getPath() const noexcept -> std::string
    {
        return path;
    }

    SettingsResult::SettingsResult(SettingsRecord_v2 record) : record{record}
    {}

    auto SettingsResult::getResult() const noexcept -> SettingsRecord_v2
    {
        return record;
    }

    auto SettingsResult::debugInfo() const -> std::string
    {
        return "GetResult";
    }

} // namespace db::query::settings
