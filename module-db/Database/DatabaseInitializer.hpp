// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database.hpp"
#include <fstream>
#include <filesystem>

class DatabaseInitializer
{
    class ScopedFile
    {
      public:
        ScopedFile(std::string path, std::string mode)
        {
            file = std::fopen(path.c_str(), mode.c_str());
        }

        ~ScopedFile()
        {
            std::fclose(file);
        }

        [[nodiscard]] std::FILE *get() const
        {
            return file;
        }

      private:
        std::FILE *file = nullptr;
    };

  public:
    DatabaseInitializer(Database *db);
    ~DatabaseInitializer() = default;

    auto run(std::filesystem::path path, std::string ext = "sql") -> bool;

    auto readCommands(std::filesystem::path filePath) -> std::vector<std::string>;

    auto listFiles(std::filesystem::path path, std::string prefix, std::string ext)
        -> std::vector<std::filesystem::path>;

    auto executeOnDb(const std::vector<std::string> statements) -> bool;

  private:
    /*
     * Splits filename in format <prefix>_<num>.ext into array
     *  [0] - filename
     *  [1] - prefix
     *  [2] - num
     */
    auto splitFilename(std::string filename) -> std::array<std::string, 3>;
    std::string readContent(const char *filename) const noexcept;

    Database *db = nullptr;
};
