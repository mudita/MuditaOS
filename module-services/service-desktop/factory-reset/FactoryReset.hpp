#pragma once

#include "Service/Service.hpp"

class FactoryReset
{
  public:
    static bool Run(sys::Service *ownerService);

  private:
    FactoryReset(){};
    ~FactoryReset(){};
    static bool DeleteDirContent(std::string dir);
    static bool CopyDirContent(std::string sourcedir, std::string targetdir);
    static bool CopyFile(std::string sourcefile, std::string targetfile);
};
