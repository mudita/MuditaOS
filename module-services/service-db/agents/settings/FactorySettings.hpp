// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <purefs/filesystem_paths.hpp>
#include <json11.hpp>
#include <fstream>
#include "Settings_queries.hpp"

namespace settings
{
    namespace factory
    {
        constexpr auto entry_key = "factory_data";
    }

    class FactorySettings
    {
      public:
        FactorySettings(std::string path) : filePath(path)
        {}

        [[nodiscard]] std::unique_ptr<QueryResult> getMfgEntries();
        void initDb(Database *database);

      private:
        json11::Json readMfgSettings(const std::string &path);
        std::string readFileContent(const std::string &filename) const noexcept;

        std::string filePath;
    };

} // namespace settings
