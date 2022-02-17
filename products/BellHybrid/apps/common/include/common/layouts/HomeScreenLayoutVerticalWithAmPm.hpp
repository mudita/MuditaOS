// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutVertical.hpp"

namespace style::homescreen_vertical_ampm
{
    constexpr inline auto alarm_margin_top   = 83U;
    constexpr inline auto battery_margin_bot = 68U;
    constexpr inline auto fmt_margin_bot     = 77U;
} // namespace style::homescreen_vertical_ampm

namespace gui
{

    class HomeScreenLayoutVerticalWithAmPm : public HomeScreenLayoutVertical
    {
      public:
        HomeScreenLayoutVerticalWithAmPm(std::string name);

        auto setTime(std::time_t newTime) -> void override;

        TextFixedSize *fmt{};
    };
}; // namespace gui
