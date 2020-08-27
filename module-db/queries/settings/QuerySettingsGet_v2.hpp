#pragma once

#include "module-db/Interface/SettingsRecord_v2.hpp"
#include <Common/Query.hpp>
#include <string>
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>

namespace db::query::settings
{
    class SettingsQuery : public Query
    {
        const std::string path;

      public:
        SettingsQuery(std::string path);

        [[nodiscard]] auto getPath() const noexcept -> std::string;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SettingsResult : public QueryResult
    {
        SettingsRecord_v2 record;

      public:
        SettingsResult(SettingsRecord_v2 record);
        [[nodiscard]] auto getResult() const noexcept -> SettingsRecord_v2;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::settings
