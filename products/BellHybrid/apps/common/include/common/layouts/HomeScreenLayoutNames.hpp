// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui
{
    namespace layout
    {
        constexpr auto Classic               = "Classic";
        constexpr auto ClassicWithBattery    = "ClassicWithBattery";
        constexpr auto ClassicWithAmPm       = "ClassicWithAmPm";
        constexpr auto ClassicWithDate       = "ClassicWithDate";
        constexpr auto ClassicWithQuotes     = "ClassicWithQuotes";
        constexpr auto ClassicWithQuotesAmPm = "ClassicWithQuotesAmPm";
        constexpr auto VerticalSimple        = "VerticalSimple";
        constexpr auto VerticalWithDate      = "VerticalWithDate";
        constexpr auto VerticalWithAmPm      = "VerticalWithAmPm";
        constexpr auto VerticalWithDateAmPm  = "VerticalWithDateAmPm";

#if CONFIG_ENABLE_TEMP == 1
        constexpr auto ClassicWithTemp = "ClassicWithTemp";
#endif

    } // namespace layout
} // namespace gui
