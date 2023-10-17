// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace style::homescreen_classic
{
    constexpr inline auto ampm_layout_w      = 51U;
    constexpr inline auto ampm_margin_w      = 5U;
    constexpr inline auto ampm_time_margin_w = ampm_layout_w + ampm_margin_w;
} // namespace style::homescreen_classic
namespace gui
{
    class HomeScreenLayoutClassicWithDate : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithDate(std::string name);

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;

      protected:
        void buildInterface() override;
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;

        Text *date          = nullptr;
        TextFixedSize *ampm = nullptr;
        bool showAMPM       = false;
    };
}; // namespace gui
