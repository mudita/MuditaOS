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
        constexpr auto battery_widget_w   = 184U;

    } // namespace battery

    enum class BatteryPercentMode
    {
        Show, // Show always
        Hide  // Never show percentage
    };

    class BellBattery : public gui::HBox
    {
      public:
        BellBattery(Item *parent);
        void setFont(const UTF8 &fontName);
        void update(units::SOC soc, bool isCharging);
        void setBatteryPercentMode(BatteryPercentMode mode);

      private:
        void setWidthsToFitContent();

        BatteryPercentMode batteryPercentMode = BatteryPercentMode::Show;
        Text *percentText                     = nullptr;
        Image *img                            = nullptr;
    };
} // namespace gui
