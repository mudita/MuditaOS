// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Service.hpp"

namespace FactoryReset
{
    bool Run(sys::Service *ownerService);
    bool DeleteDirContent(std::string dir);
    bool CopyDirContent(std::string sourcedir, std::string targetdir);
} // namespace FactoryReset
