// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Platform.hpp>

#include <purefs/vfs_subsystem.hpp>
#include <purefs/filesystem_paths.hpp>
#include <filesystem>

namespace platform
{

    void Platform::initCommonUserFolders()
    {
        if (!std::filesystem::exists(purefs::dir::getLogsPath())) {
            std::filesystem::create_directories(purefs::dir::getLogsPath());
        }

        if (!std::filesystem::exists(purefs::dir::getCrashDumpsPath())) {
            std::filesystem::create_directories(purefs::dir::getCrashDumpsPath());
        }
    }
} // namespace platform
