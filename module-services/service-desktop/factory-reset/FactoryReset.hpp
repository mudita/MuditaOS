#pragma once

#include "Service/Service.hpp"

class FactoryReset
{
  public:
    static void Run(sys::Service *ownerService);

  private:
    FactoryReset(){};
    ~FactoryReset(){};
    static void DeleteDirContent(std::string dir);
    static void CopyDirContent(std::string sourcedir, std::string targetdir);
    static void CopyFile(std::string sourcefile, std::string targetfile);
};
