// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutVertical.hpp"

namespace style::homescreen_vertical_date
{
    inline constexpr auto margin_top{64U};
    inline constexpr auto margin_bot{70U};
} // namespace style::homescreen_vertical_date

namespace gui
{
    class HomeScreenLayoutVerticalWithDate : public HomeScreenLayoutVertical
    {
      public:
        explicit HomeScreenLayoutVerticalWithDate(std::string name);

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;

      private:
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;
        bool isAlarmTimeVisibilityAllowed() override;

        TextFixedSize *ampm = nullptr;
        TextFixedSize *date = nullptr;
    };
} // namespace gui
