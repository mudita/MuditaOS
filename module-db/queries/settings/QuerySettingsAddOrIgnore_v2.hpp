#pragma once

#include "module-db/Interface/SettingsRecord_v2.hpp"
#include <Common/Query.hpp>

namespace db::query::settings
{
    class AddOrIgnoreQuery : public Query
    {
        const SettingsRecord_v2 record;

      public:
        AddOrIgnoreQuery(const SettingsRecord_v2 &record);

        [[nodiscard]] auto getRecord() const noexcept -> const SettingsRecord_v2 &;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class AddOrIgnoreResult : public QueryResult
    {
        bool value = false;

      public:
        AddOrIgnoreResult(const bool &value);

        [[nodiscard]] auto getValue() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::settings
