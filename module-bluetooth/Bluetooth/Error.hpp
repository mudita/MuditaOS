#pragma once

#include <cstdint>
#include <variant>
#include <optional>

namespace Bt
{

    struct Error
    {
        enum Code
        {
            Succes,
            NotReady,
            SystemError,
            LibraryError,
        } err             = Succes;
        uint32_t lib_code = 0;
        Error(enum Code err = Succes, int lib_code = Succes) : err(err), lib_code(0)
        {}
    };

} // namespace Bt
