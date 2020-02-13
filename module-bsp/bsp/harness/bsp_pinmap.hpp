#pragma once

namespace bsp::harness::gpio
{
    enum Pin : int
    {
        SIM_SELECT = 0,
        SIM_PRESENCE = 1,
        NONE,
    };
}
