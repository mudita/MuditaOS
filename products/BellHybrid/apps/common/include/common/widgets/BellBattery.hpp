// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <EventStore.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>

namespace gui
{

    namespace battery
    {
        constexpr auto battery_low        = "bell_battery_lvl1";
        constexpr auto battery_critical   = "bell_battery_empty";
        constexpr auto battery_charging   = "bell_battery_charging";
        constexpr auto font_small         = style::window::font::largelight;
        constexpr auto image_h            = 64;
        constexpr auto image_w            = 64U;
        constexpr auto image_left_margin  = 0U;
        constexpr auto image_right_margin = 10U;
        constexpr auto percent_h          = 102U;
        constexpr auto percent_w          = 106U;
        constexpr auto battery_widget_h   = 64U;
        constexpr auto battery_widget_w   = 240U;

    } // namespace battery

    enum class BatteryPercentMode
    {
        Dynamic,
        Static,
    };

    class BellBattery : public gui::HBox
    {
      public:
        BellBattery(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update(const Store::Battery &batteryContext);
        void setBatteryPercentMode(BatteryPercentMode mode);
        std::uint32_t getLevel();

      private:
        BatteryPercentMode batteryPercentMode = BatteryPercentMode::Dynamic;
        TextFixedSize *percentText;
        Image *img;
    };
} // namespace gui
