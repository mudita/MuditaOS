// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp
{
    enum rebootCode : std::uint32_t
    {
        rebootNormalCode       = std::uint32_t{0},
        rebootToUpdateCode     = std::uint32_t{0xdead0000},
        rebootToRecoveryCode   = std::uint32_t{0xdead0001},
        rebootToFactoryRstCode = std::uint32_t{0xdead0002},
        rebootFailedToBoot     = std::uint32_t{0xdeadFFFF}
    };
} // namespace bsp
