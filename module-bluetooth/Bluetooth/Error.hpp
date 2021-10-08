// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <variant>
#include <optional>

namespace bluetooth
{

    struct Error
    {
        enum Code
        {
            Success,
            NotReady,
            SystemError,
            LibraryError,
        } err             = Success;
        uint32_t lib_code = 0;
        Error(enum Code err = Success, int lib_code = Success) : err(err)
        {}
    };

} // namespace Bt
const char *c_str(bluetooth::Error::Code code);
