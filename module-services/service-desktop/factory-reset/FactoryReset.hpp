#pragma once

#include "Service/Service.hpp"

namespace FactoryReset
{
    bool Run(sys::Service *ownerService);
    bool DeleteDirContent(std::string dir);
    bool CopyDirContent(std::string sourcedir, std::string targetdir);
} // namespace FactoryReset
