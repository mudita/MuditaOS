// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ProductConfig.hpp>

namespace gui::layout
{
    inline constexpr auto Classic               = "Classic";
    inline constexpr auto ClassicWithBattery    = "ClassicWithBattery";
    inline constexpr auto ClassicWithAmPm       = "ClassicWithAmPm";
    inline constexpr auto ClassicWithDate       = "ClassicWithDate";
    inline constexpr auto ClassicWithQuotes     = "ClassicWithQuotes";
    inline constexpr auto ClassicWithQuotesAmPm = "ClassicWithQuotesAmPm";
    inline constexpr auto VerticalSimple        = "VerticalSimple";
    inline constexpr auto VerticalWithDate      = "VerticalWithDate";
    inline constexpr auto VerticalWithAmPm      = "VerticalWithAmPm";
    inline constexpr auto VerticalWithDateAmPm  = "VerticalWithDateAmPm";

#if CONFIG_ENABLE_TEMP == 1
    inline constexpr auto ClassicWithTemp = "ClassicWithTemp";
#endif
} // namespace gui
