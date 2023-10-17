// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <purefs/filesystem_paths.hpp>
#include <json11.hpp>
#include <fstream>
#include "service-db/agents/settings/FactorySettings.hpp"

namespace settings
{
    namespace factory
    {
        static constexpr auto device_version_key = "device_version";
    }

    class BellFactorySettings : public FactorySettings
    {
      public:
        [[nodiscard]] auto getMfgEntries() const -> std::unique_ptr<QueryResult> override final;
        auto initDb(Database *database) const -> void override final;
    };
} // namespace settings
