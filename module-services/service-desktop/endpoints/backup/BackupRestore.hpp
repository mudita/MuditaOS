// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <filesystem>

namespace sys
{
    class Service;
} // namespace sys

class BackupRestore
{
  public:
    BackupRestore(){};
    ~BackupRestore(){};
    static bool BackupUserFiles(sys::Service *ownerService, std::filesystem::path &path);
    static void RestoreUserFiles(sys::Service *ownerService);

  private:
    static bool RemoveBackupDir(std::filesystem::path &path);
    static bool CreateBackupDir(std::filesystem::path &path);
    static bool PackUserFiles(std::filesystem::path &path);
    static bool UnpackBackupFile();
    static bool ReplaceUserFiles();
    static bool WriteBackupInfo(sys::Service *ownerService, const std::filesystem::path &path);
};
