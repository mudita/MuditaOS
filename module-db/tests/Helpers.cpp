// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Helpers.hpp"

#include <Utils.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

namespace
{
    std::string readContent(const std::string &filename) noexcept
    {
        auto getFileSize = [](FILE *fd) -> std::size_t {
            std::fseek(fd, 0, SEEK_END);
            const auto size = std::ftell(fd);
            std::rewind(fd);
            return size;
        };

        std::vector<char> fileContent;
        if (const auto fp = std::fopen(filename.c_str(), "r")) {
            const auto fileSize = getFileSize(fp);

            fileContent.reserve(fileSize + 1);
            std::fread(fileContent.data(), 1, fileSize, fp);
            std::fclose(fp);
            fileContent[fileSize] = '\0';
            return fileContent.data();
        }
        return {};
    }

    std::vector<std::string> readCommands(const std::filesystem::path &filePath)
    {
        const auto fileContent = readContent(filePath.c_str());
        std::string currentStatement{};
        std::vector<std::string> statements{};

        std::string line{};
        for (const auto &c : fileContent) {
            if (c != '\n') {
                line += c;
            }
            else {
                if (line.empty() || utils::startsWith(line, "--")) {
                    line.clear();
                    continue;
                }
                if (utils::endsWith(line, ";")) {
                    statements.push_back(currentStatement + line);
                    currentStatement.clear();
                    line.clear();
                    continue;
                }
                currentStatement += line;

                line.clear();
            }
        }
        return statements;
    }

    std::set<std::filesystem::path> listVersionDirectories(const std::filesystem::path &path, const std::string &dbName)
    {
        std::set<std::filesystem::path> versions;
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory() and entry.path().filename() == dbName) {
                for (const auto &version : std::filesystem::directory_iterator(entry.path())) {
                    versions.insert(version.path());
                }
            }
        }
        return versions;
    }

    std::vector<std::filesystem::path> listFiles(const std::filesystem::path &path,
                                                 const std::string &prefix,
                                                 const bool withDevelopment)
    {
        constexpr auto up_sql    = "up.sql";
        constexpr auto devel_sql = "devel.sql";
        std::vector<std::filesystem::path> files;
        for (const auto &version : listVersionDirectories(path, prefix)) {
            files.push_back(version / up_sql);
            if (withDevelopment and std::filesystem::exists(version / devel_sql)) {
                files.push_back(version / devel_sql);
            }
        }
        return files;
    }

    bool executeOnDb(Database &db, const std::vector<std::string> &statements)
    {
        return std::all_of(statements.begin(), statements.end(), [&db](const auto &statement) {
            return db.execute(statement.c_str());
        });
    }
} // namespace

namespace db::tests
{
    std::filesystem::path currentFileLocation()
    {
        const std::string path = __FILE__;
        return std::filesystem::path{path.substr(0, path.rfind('/'))};
    }

    std::filesystem::path getScriptsPath()
    {
        return currentFileLocation() / "../databases/migration";
    }

    /// TODO:
    /// This function is obviously wrong and should not exist in the first place. Unfortunately, due to the
    /// current directory structure, product specific modules and some unit tests are placed in common directories.
    /// After proper module-db and service-db split it wont' be needed as every product-specific db unit test will be
    /// placed in the correct directory.
    std::filesystem::path getPurePhoneScriptsPath()
    {
        return currentFileLocation() / "../../products/PurePhone/services/db/databases/migration";
    }

    bool DatabaseScripts::operator()(Database &db)
    {
        const std::filesystem::path dbpath = db.getName();
        const std::string dbname           = dbpath.filename().replace_extension();

        const auto scripts = listFiles(directory, dbname, withDevelopment);
        return std::all_of(scripts.begin(), scripts.end(), [&db](const auto &script) {
            return executeOnDb(db, readCommands(script));
        });
    }
    DatabaseScripts::DatabaseScripts(std::filesystem::path directory, bool withDevelopment)
        : directory{std::move(directory)}, withDevelopment{withDevelopment}
    {}
} // namespace db::tests