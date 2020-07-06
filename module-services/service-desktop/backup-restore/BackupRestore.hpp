#pragma once

#include "Service/Service.hpp"

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
