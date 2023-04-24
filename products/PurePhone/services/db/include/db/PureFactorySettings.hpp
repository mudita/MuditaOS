// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <purefs/filesystem_paths.hpp>
#include <json11.hpp>
#include <fstream>
#include <service-db/agents/settings/FactorySettings.hpp>

namespace settings
{
    class PureFactorySettings : public FactorySettings
    {
      public:
        explicit PureFactorySettings(const std::string &filePath);
        [[nodiscard]] auto getMfgEntries() const -> std::unique_ptr<QueryResult> override final;
        auto initDb(Database *database) const -> void override final;

      private:
        const std::string filePath;
        auto readMfgSettings() const -> json11::Json;
    };
} // namespace settings
