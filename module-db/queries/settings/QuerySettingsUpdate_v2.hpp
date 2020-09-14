#pragma once

#include "module-db/Interface/SettingsRecord_v2.hpp"
#include <Common/Query.hpp>

namespace db::query::settings
{
    class UpdateQuery : public Query
    {
        const SettingsRecord_v2 record;

      public:
        UpdateQuery(const SettingsRecord_v2 &record);

        [[nodiscard]] auto debugInfo() const -> std::string override;
        [[nodiscard]] auto getRecord() const -> const SettingsRecord_v2 &;
    };

    class UpdateResult : public QueryResult
    {
        bool value = false;

      public:
        UpdateResult(const bool &value);

        [[nodiscard]] auto getValue() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };
} // namespace db::query::settings
