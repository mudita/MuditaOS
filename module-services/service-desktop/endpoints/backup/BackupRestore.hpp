// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <filesystem>
#include <json11.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace bkp
{
    inline constexpr auto backupInfo = "backup.json";
};

class BackupRestore
{
  public:
    BackupRestore(){};
    ~BackupRestore(){};
    static bool BackupUserFiles(sys::Service *ownerService, std::filesystem::path &path);
    static bool RestoreUserFiles(sys::Service *ownerService, const std::filesystem::path &path);
    static json11::Json GetBackupFiles();

  private:
    static bool RemoveBackupDir(std::filesystem::path &path);
    static bool CreateBackupDir(std::filesystem::path &path);
    static bool PackUserFiles(std::filesystem::path &path);
    static bool UnpackBackupFile(const std::filesystem::path &path);
    static bool ReplaceUserFiles(const std::filesystem::path &path);
    static bool WriteBackupInfo(sys::Service *ownerService, const std::filesystem::path &path);
};
