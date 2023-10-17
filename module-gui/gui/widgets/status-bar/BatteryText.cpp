// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryText.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include <i18n/i18n.hpp>
#include <Utils.hpp>

namespace gui::status_bar
{
    BatteryText::BatteryText(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : BatteryBase(parent, x, y, w, h)
    {
        label = new Label(this, 0, 0, 0, 0);
        label->setFilled(false);
        label->setBorderColor(gui::ColorNoColor);
        label->setFont(style::status_bar::battery::font);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Bottom));

        setMinimumSize(style::status_bar::battery::maxX, style::status_bar::height);
        label->setMaximumSize(style::status_bar::battery::maxX, style::status_bar::height);
    }

    void BatteryText::showBatteryLevel(std::uint32_t percentage)
    {
        label->setText(utils::to_string(percentage) + " %");
    }

    void BatteryText::showBatteryCharging()
    {
        label->setText(utils::translate("statusbar_battery_charging"));
    }

    void BatteryText::showBatteryChargingDone()
    {
        label->setText(utils::translate("statusbar_battery_plugged"));
    }

} // namespace gui::status_bar
