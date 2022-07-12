// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>

namespace gui
{

    namespace battery
    {
        constexpr auto font_small         = style::window::font::largelight;
        constexpr auto image_right_margin = 10U;
        constexpr auto percent_h          = 102U;
        constexpr auto percent_w          = 106U;
        constexpr auto battery_widget_h   = 64U;
        constexpr auto battery_widget_w   = 240U;

    } // namespace battery

    enum class BatteryPercentMode
    {
        Show, // Show always
        Hide  // Never show percentage
    };

    class BellBattery : public gui::HBox
    {
      public:
        BellBattery(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update(units::SOC soc, bool isCharging);
        void setBatteryPercentMode(BatteryPercentMode mode);

      private:
        BatteryPercentMode batteryPercentMode = BatteryPercentMode::Show;
        TextFixedSize *percentText;
        Image *img;
    };
} // namespace gui
