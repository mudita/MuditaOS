// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Helpers.hpp"

#include <Utils.hpp>
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

    std::array<std::string, 3> splitFilename(const std::string &filename)
    {
        const auto name    = filename.substr(0, filename.find("."));
        const auto prefix  = name.substr(0, name.find_last_of("_"));
        const auto postfix = name.substr(name.find_last_of("_") + 1, std::string::npos);

        return {name, prefix, postfix};
    }

    bool isOnExcludedList(const std::string &name, const std::vector<std::string> &excludedList)
    {
        return std::any_of(excludedList.begin(), excludedList.end(), [&name](const auto &entry) {
            return name.find(entry) != std::string::npos;
        });
    }

    std::vector<std::filesystem::path> listFiles(const std::filesystem::path &path,
                                                 const std::string &prefix,
                                                 const std::vector<std::string> &excludedList)
    {
        std::set<std::pair<int, std::filesystem::path>> orderedFiles;
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (!entry.is_directory() && entry.path().has_filename()) {
                try {
                    const auto parts       = splitFilename(entry.path().filename().string());
                    const auto &filePrefix = parts[1];
                    if (filePrefix == prefix and not isOnExcludedList(parts[0], excludedList)) {
                        orderedFiles.insert({std::stoi(parts[2]), entry.path()});
                    }
                }
                catch (std::invalid_argument &e) {
                    printf("Ignoring file: %s", entry.path().c_str());
                }
            }
        }

        std::vector<std::filesystem::path> files;
        std::for_each(orderedFiles.begin(), orderedFiles.end(), [&](auto item) { files.push_back(item.second); });
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
        return currentFileLocation() / "../databases/scripts";
    }

    /// TODO:
    /// This function is obviously wrong and should not exist in the first place. Unfortunately, due to the
    /// current directory structure, product specific modules and some unit tests are placed in common directories.
    /// After proper module-db and service-db split it wont' be needed as every product-specific db unit test will be
    /// placed in the correct directory.
    std::filesystem::path getPurePhoneScriptsPath()
    {
        return currentFileLocation() / "../../products/PurePhone/services/db/databases/scripts";
    }

    bool DatabaseScripts::operator()(Database &db)
    {
        const std::filesystem::path dbpath = db.getName();
        const std::string dbname           = dbpath.filename().replace_extension();

        const auto scripts = listFiles(directory, dbname, exclude);
        return std::all_of(scripts.begin(), scripts.end(), [&db](const auto &script) {
            return executeOnDb(db, readCommands(script));
        });
    }
    DatabaseScripts::DatabaseScripts(std::filesystem::path directory, std::vector<std::string> &&exclude)
        : directory{std::move(directory)}, extension{".sql"}, exclude{std::move(exclude)}
    {}
} // namespace db::tests