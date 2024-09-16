// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutVertical.hpp"

namespace style::homescreen_vertical_ampm
{
    inline constexpr auto alarm_margin_top{83U};
    inline constexpr auto battery_margin_bot{68U};
    inline constexpr auto ampm_margin_bot{77U};
} // namespace style::homescreen_vertical_ampm

namespace gui
{

    class HomeScreenLayoutVerticalWithAmPm : public HomeScreenLayoutVertical
    {
      public:
        HomeScreenLayoutVerticalWithAmPm(std::string name);

        auto setTime(std::time_t newTime) -> void override;

        TextFixedSize *ampm{};
    };
} // namespace gui
