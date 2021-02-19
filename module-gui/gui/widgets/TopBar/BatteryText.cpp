// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryText.hpp"
#include "Label.hpp"
#include <Utils.hpp>
#include <Style.hpp>

namespace gui::top_bar
{
    BatteryText::BatteryText(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : BatteryBase(parent, x, y, w, h)
    {
        label = new Label(this, 0, 0, 0, 0);
        label->setFilled(false);
        label->setBorderColor(gui::ColorNoColor);
        label->setFont(style::header::font::modes);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->setMaximumSize(this->getWidth(), this->getHeight());
    }

    void BatteryText::showBatteryLevel(std::uint32_t percentage)
    {
        label->setText(utils::to_string(percentage) + " %");
    }

    void BatteryText::showBatteryCharging()
    {
        label->setText(utils::localize.get("topbar_battery_charging"));
    }

    void BatteryText::showBatteryChargingDone()
    {
        label->setText(utils::localize.get("topbar_battery_plugged"));
    }

} // namespace gui::top_bar
