// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DatabaseInitializer.hpp"

#include <vfs.hpp>
#include <memory>
#include <set>

DatabaseInitializer::DatabaseInitializer(Database *db) : db(db)
{}

bool DatabaseInitializer::run(fs::path path, std::string ext)
{
    // Database name is database file path, need to strip off all filesystem related stuff(path, extension)
    fs::path dbpath    = db->getName();
    std::string dbname = dbpath.filename().replace_extension();

    auto files = listFiles(path, dbname, ext);
    for (auto file : files) {
        LOG_DEBUG("Runing db script: %s", file.c_str());
        auto commands = readCommands(file);
        if (!executeOnDb(commands)) {
            LOG_ERROR("Can't initialize database [%s] with [%s]", db->getName().c_str(), file.c_str());
            return false;
        }
    }
    return true;
}

std::vector<std::string> DatabaseInitializer::readCommands(fs::path filePath)
{
    ScopedFile file(filePath, "r");

    std::string line;
    std::string currentStatement;
    std::vector<std::string> statements;

    while (!vfs.eof(file.get())) {
        line = vfs.getline(file.get());

        if (line.empty() || starts_with(line, std::string("--"))) {
            continue;
        }

        if (ends_with(line, std::string(";"))) {
            statements.push_back(currentStatement + line);
            currentStatement.clear();
            continue;
        }

        currentStatement += line;
    }

    return statements;
}

std::array<std::string, 3> DatabaseInitializer::splitFilename(std::string filename)
{
    auto name    = filename.substr(0, filename.find("."));
    auto prefix  = name.substr(0, name.find("_"));
    auto postfix = name.substr(name.find("_") + 1, std::string::npos);

    return {name, prefix, postfix};
}

std::vector<fs::path> DatabaseInitializer::listFiles(fs::path path, std::string prefix, std::string ext)
{
    std::set<std::pair<int, fs::path>> orderedFiles;
    auto dirList = vfs.listdir(path.c_str(), ext);
    for (vfs::DirectoryEntry ent : dirList) {
        if (ent.attributes != vfs::FileAttributes::Directory) {
            try {
                auto parts      = splitFilename(ent.fileName);
                auto filePrefix = parts[1];
                if (filePrefix == prefix) {
                    auto num = std::stoi(parts[2]);
                    orderedFiles.insert({num, path / ent.fileName});
                }
            }
            catch (std::invalid_argument &e) {
                LOG_INFO("Ignoring file: %s", ent.fileName.c_str());
            }
        }
    }

    std::vector<fs::path> files;
    std::for_each(orderedFiles.begin(), orderedFiles.end(), [&](auto item) { files.push_back(item.second); });
    return files;
}

bool DatabaseInitializer::executeOnDb(const std::vector<std::string> statements)
{
    for (auto st : statements) {
        if (!db->execute(st.c_str())) {
            return false;
        }
    }
    return true;
}
