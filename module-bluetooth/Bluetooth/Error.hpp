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
            Success,
            NotReady,
            SystemError,
            LibraryError,
        } err             = Success;
        uint32_t lib_code = 0;
        Error(enum Code err = Success, int lib_code = Success) : err(err), lib_code(0)
        {}
    };

} // namespace Bt
const char *c_str(Bt::Error::Code code);
