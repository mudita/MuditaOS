// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverOscillator.hpp"
#include "board/rt1051/bsp/lpm/Oscillator.hpp"

namespace drivers
{

    RT1051DriverOscillator::RT1051DriverOscillator() noexcept
    {
        bsp::EnableExternalOscillator();
    }

} // namespace drivers
