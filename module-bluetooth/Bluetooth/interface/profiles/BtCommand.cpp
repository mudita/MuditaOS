// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BtCommand.hpp"

#include <array>
#include <cstdint>

extern "C"
{
#include <gap.h>
}

namespace bluetooth
{
    // Set local name with a template Bluetooth address, that will be automatically
    // replaced with a actual address once it is available, i.e. when BTstack boots
    Error set_name(std::string &name)
    {
        // name has to have storage
        constexpr std::uint32_t size = 64;
        static std::array<char, size> lname;
        snprintf(lname.data(), size, "%s", name.c_str());
        gap_set_local_name(lname.data());
        return Error();
    }
} // namespace bluetooth
