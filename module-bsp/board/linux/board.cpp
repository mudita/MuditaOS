// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

namespace bsp
{
    void board_init()
    {
        // dummy
    }

    void board_power_off()
    {
        // dummy
    }

    void board_restart()
    {
        // dummy
    }

} // namespace bsp

namespace bsp::bell_switches
{
    bool isLatchPressed()
    {
        return false;
    }
} // namespace bsp::bell_switches
