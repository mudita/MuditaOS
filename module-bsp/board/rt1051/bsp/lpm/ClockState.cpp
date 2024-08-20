// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ClockState.hpp"
#include <cstdint>

namespace bsp
{
    constexpr std::uint8_t CCM_TupleShift{8};
    constexpr std::uint8_t CCM_TupleMask{0x1F};
    constexpr std::uint32_t ClockNeededRunWaitMode{kCLOCK_ClockNeededRunWait};

    bool IsClockEnabled(clock_ip_name_t name)
    {
        const auto index = static_cast<std::uint32_t>(name) >> CCM_TupleShift;
        const auto shift = static_cast<std::uint32_t>(name) & CCM_TupleMask;

        return ((*reinterpret_cast<volatile std::uint32_t *>(&CCM->CCGR0 + index)) & (ClockNeededRunWaitMode << shift));
    }
} // namespace bsp
