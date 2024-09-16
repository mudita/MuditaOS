// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "Helpers.hpp"

#include <Utils.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <fstream>

namespace
{
    std::string readContent(const std::string &filename)
    {
        std::ifstream file{filename};
        if (!file.is_open()) {
            return {};
        }

        const auto fileSize = std::filesystem::file_size(filename);
        auto fileContent    = std::make_unique<char[]>(fileSize + 1);

        file.read(fileContent.get(), fileSize);
        fileContent[fileSize] = '\0';

        return std::string{fileContent.get()};
    }

    std::vector<std::string> readCommands(const std::filesystem::path &filePath)
    {
        const auto &fileContent = readContent(filePath.c_str());
        std::string currentStatement{};
        std::vector<std::string> statements{};

        std::string line{};
        for (const auto c : fileContent) {
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

    std::vector<std::filesystem::path> searchForScripts(const std::filesystem::path &pathToVersion,
                                                        const std::string &scriptName)
    {
        std::vector<std::filesystem::path> scripts{};
        if (std::filesystem::exists(pathToVersion / scriptName)) {
            scripts.push_back(pathToVersion / scriptName);
        }
        else {
            for (const auto &subVersion : std::filesystem::directory_iterator(pathToVersion)) {
                if (std::filesystem::exists(subVersion.path() / scriptName)) {
                    scripts.push_back(subVersion.path() / scriptName);
                }
            }
        }
        return scripts;
    }

    std::vector<std::filesystem::path> listFiles(const std::filesystem::path &path,
                                                 const std::string &prefix,
                                                 const bool withDevelopment)
    {
        constexpr auto up_sql    = "up.sql";
        constexpr auto devel_sql = "devel.sql";
        std::vector<std::filesystem::path> files;
        for (const auto &version : listVersionDirectories(path, prefix)) {
            auto scriptsPath = searchForScripts(version, up_sql);
            if (not scriptsPath.empty()) {
                files.insert(files.end(), scriptsPath.begin(), scriptsPath.end());
            }
            if (withDevelopment) {
                scriptsPath = searchForScripts(version, devel_sql);
                if (not scriptsPath.empty()) {
                    files.insert(files.end(), scriptsPath.begin(), scriptsPath.end());
                }
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