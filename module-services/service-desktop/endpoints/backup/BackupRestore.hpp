// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Service.hpp"

namespace sys
{
    class Service;
} // namespace sys

class BackupRestore
{
  public:
    BackupRestore(){};
    ~BackupRestore(){};
    static void BackupUserFiles(sys::Service *ownerService);
    static void RestoreUserFiles(sys::Service *ownerService);

  private:
    static bool RemoveBackupDir();
    static bool CreateBackupDir();
    static bool PackUserFiles();
    static bool UnpackBackupFile();
    static bool ReplaceUserFiles();
};
