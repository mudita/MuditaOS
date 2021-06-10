// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryBar.hpp"
#include "Style.hpp"
#include "Utils.hpp"
#include "visitor/GuiVisitor.hpp"
#include <Image.hpp>

namespace gui::status_bar
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

        constexpr auto level1Threshold = 15;
        constexpr auto level2Threshold = 35;
        constexpr auto level3Threshold = 55;
        constexpr auto level4Threshold = 75;
        constexpr auto level5Threshold = 95;
    } // namespace

    BatteryBar::BatteryBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : BatteryBase(parent, x, y, w, h)
    {
        img = new Image(this, battery1);

        setMinimumSize(img->getWidth(), style::header::status_bar::height);
    }

    void BatteryBar::showBatteryLevel(std::uint32_t percentage)
    {
        if (percentage < level1Threshold) {
            img->set(batteryLow);
        }
        else if (percentage < level2Threshold) {
            img->set(battery1);
        }
        else if (percentage < level3Threshold) {
            img->set(battery2);
        }
        else if (percentage < level4Threshold) {
            img->set(battery3);
        }
        else if (percentage < level5Threshold) {
            img->set(battery4);
        }
        else {
            img->set(battery5);
        }
    }

    void BatteryBar::showBatteryCharging()
    {
        img->set(batteryCharging);
    }

    void BatteryBar::showBatteryChargingDone()
    {
        img->set(batteryChargingReady);
    }

} // namespace gui::status_bar
