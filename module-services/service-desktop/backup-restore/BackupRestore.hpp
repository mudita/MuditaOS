#pragma once

#include "Service/Service.hpp"

class BackupRestore
{
  public:
    BackupRestore(){};
    ~BackupRestore(){};
    static void BackupUserFiles(sys::Service *ownerService);

  private:
    static bool CleanBackupDir();
    static bool CreateBackupDir();
    bool OpenTarFile();
};
