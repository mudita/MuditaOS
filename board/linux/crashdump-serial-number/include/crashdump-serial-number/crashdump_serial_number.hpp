// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Dummy file to keep linux variant compiling.

#pragma once
#include <string>
namespace crashdump
{
    void setSerialNumber(const std::string &sn);
    std::string getSerialNumber();

} // namespace crashdump
