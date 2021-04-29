// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include "common.hpp"
#include <vector>
#include <filesystem>

void RemoveDbFiles(const std::string &dbName)
{
    std::vector<std::string> dbFileExt = {".db", ".db-journal", ".db-wal"};
    for (const auto &ext : dbFileExt) {
        const auto dbPath = (std::filesystem::path{"sys/user"} / std::filesystem::path{dbName + ext});
        if (std::filesystem::exists(dbPath)) {
            std::filesystem::remove(dbPath.c_str());
        }
    }
}
