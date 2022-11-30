// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <string>
#include <filesystem>

namespace db::tests
{
    std::filesystem::path getScriptsPath();
    std::filesystem::path getPurePhoneScriptsPath();

    class DatabaseScripts
    {
      public:
        DatabaseScripts(std::filesystem::path directory, bool withDevelopment);
        bool operator()(Database &db);

      private:
        std::filesystem::path directory;
        bool withDevelopment;
    };

    template <typename Db>
    class DatabaseUnderTest
    {
      public:
        DatabaseUnderTest(std::filesystem::path directory,
                          std::filesystem::path scriptsPath,
                          bool withDevelopment = false)
        {
            Database::initialize();

            std::filesystem::remove(directory);

            /// Some databases initialize internal fields in the constructors. To avoid fetching wrong data from
            /// uninitialized database fields spawn temporary database, execute any available scripts and finally
            /// recreate it. This way we are sure database will load correct data.
            {
                auto tempDb = std::make_unique<Db>(directory.c_str());

                if (not tempDb->isInitialized()) {
                    throw std::runtime_error("Could not initialize database");
                }

                if (not scriptsPath.empty() and not DatabaseScripts{scriptsPath, withDevelopment}(*tempDb)) {
                    throw std::runtime_error("Failed to execute database scripts");
                }
            }

            db = std::make_unique<Db>(directory.c_str());
        }

        explicit DatabaseUnderTest(std::filesystem::path directory) : DatabaseUnderTest(directory, {})
        {}

        ~DatabaseUnderTest()
        {
            Database::deinitialize();
        }

        Db &get()
        {
            return *db;
        }

      private:
        std::unique_ptr<Db> db;
    };
} // namespace db::tests
