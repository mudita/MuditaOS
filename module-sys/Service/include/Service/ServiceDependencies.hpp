// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

namespace sys::dependencies
{
    template <class T>
    std::vector<std::string> getDependenciesFor();
    template <class T>
    std::vector<std::string> getDependenciesTo();
} // namespace sys::dependencies
