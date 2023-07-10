// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bluetooth
{
    struct Result
    {
        enum class Code
        {
            Success,
            NotReady,
            SystemError,
            LibraryError,
        };

        Code result;
        int libraryResult;

        explicit Result(Code result = Code::Success, int libraryResult = 0)
            : result(result), libraryResult(libraryResult)
        {}
    };
} // namespace bluetooth
