// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database.hpp"

namespace
{
    template <typename T> inline bool starts_with(const T &str, const T &start)
    {
        if (start.size() > str.size())
            return false;
        return str.compare(0, start.size(), start) == 0;
    }
    template <typename T> inline bool ends_with(const T &str, const T &end)
    {
        if (end.size() > str.size())
            return false;
        return std::equal(end.rbegin(), end.rend(), str.rbegin());
    }
} // namespace

class DatabaseInitializer
{
    class ScopedFile
    {
      public:
        ScopedFile(std::string path, std::string mode)
        {
            file = vfs.fopen(path.c_str(), mode.c_str());
        }

        ~ScopedFile()
        {
            vfs.fclose(file);
        }

        [[nodiscard]] vfs::FILE *get() const
        {
            return file;
        }

      private:
        vfs::FILE *file = nullptr;
    };

  public:
    DatabaseInitializer(Database *db);
    ~DatabaseInitializer() = default;

    auto run(fs::path path, std::string ext = "sql") -> bool;

    auto readCommands(fs::path filePath) -> std::vector<std::string>;

    auto listFiles(fs::path path, std::string prefix, std::string ext) -> std::vector<fs::path>;

    auto executeOnDb(const std::vector<std::string> statements) -> bool;

  private:
    /*
     * Splits filename in format <prefix>_<num>.ext into array
     *  [0] - filename
     *  [1] - prefix
     *  [2] - num
     */
    auto splitFilename(std::string filename) -> std::array<std::string, 3>;

    Database *db = nullptr;
};
