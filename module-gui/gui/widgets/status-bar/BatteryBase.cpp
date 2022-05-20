// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryBase.hpp"
#include "BoxLayout.hpp"

namespace gui::status_bar
{
    BatteryBase::BatteryBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setEdges(RectangleEdge::None);
        setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Bottom));
    }

    bool BatteryBase::update(const Store::Battery &batteryContext)
    {
        switch (batteryContext.state) {
        case Store::Battery::State::Discharging:
            [[fallthrough]];
        case Store::Battery::State::PluggedNotCharging:
            showBatteryLevel(batteryContext.level);
            break;
        case Store::Battery::State::Charging:
            showBatteryCharging();
            break;
        case Store::Battery::State::ChargingDone:
            showBatteryChargingDone();
            break;
        }

        return Store::Battery::takeUpdated();
    }
} // namespace gui::status_bar
