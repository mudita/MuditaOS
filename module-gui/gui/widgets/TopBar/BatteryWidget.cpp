// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryWidget.hpp"

namespace gui
{
    namespace
    {
        constexpr auto batteryLow           = "battery_low_W_M";
        constexpr auto batteryCharging      = "battery_charging_W_M";
        constexpr auto batteryChargingReady = "battery_charging_ready_W_M";
        constexpr auto battery1             = "battery1_W_M";
        constexpr auto battery2             = "battery2_W_M";
        constexpr auto battery3             = "battery3_W_M";
        constexpr auto battery4             = "battery4_W_M";
        constexpr auto battery5             = "battery5_W_M";

        constexpr auto level1Threshold = 5;
        constexpr auto level2Threshold = 27;
        constexpr auto level3Threshold = 50;
        constexpr auto level4Threshold = 73;
        constexpr auto level5Threshold = 95;
    } // namespace

    BatteryWidget::BatteryWidget(Item *parent, uint32_t x, uint32_t y) : Image(parent, x, y, 0, 0)
    {
        set(battery1);
    }

    void BatteryWidget::show(const Store::Battery batteryContext, bool shown)
    {
        if (shown) {
            switch (batteryContext.state) {
            case Store::Battery::State::Discharging:
                showBatteryBars(batteryContext.level);
                break;
            case Store::Battery::State::Charging:
                set(batteryCharging);
                break;
            case Store::Battery::State::PluggedNotCharging:
                set(batteryChargingReady);
                break;
            }
        }
        else {
            setVisible(false);
        }
    }

    void BatteryWidget::showBatteryBars(std::uint32_t percentage)
    {
        if (percentage <= level1Threshold)
            set(batteryLow);
        else if (percentage <= level2Threshold)
            set(battery1);
        else if (percentage <= level3Threshold)
            set(battery2);
        else if (percentage <= level4Threshold)
            set(battery3);
        else if (percentage <= level5Threshold)
            set(battery4);
        else
            set(battery5);
    }

} // namespace gui
