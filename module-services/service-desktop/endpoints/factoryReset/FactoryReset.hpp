// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <purefs/filesystem_paths.hpp>
#include <Service/Service.hpp>
#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace FactoryReset
{
    bool Run(sys::Service *ownerService);
    bool DeleteSelectedUserFiles(const std::filesystem::path &userOSPath);
    bool DeleteDirContent(const std::string &dir);
    bool CopyDirContent(const std::string &sourcedir, const std::string &targetdir);
} // namespace FactoryReset
