// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include "BoxLayout.hpp"
#include <EventStore.hpp>

namespace gui::status_bar
{
    class BatteryBase : public StatusBarWidgetBase<gui::HBox>
    {
        virtual void showBatteryLevel(std::uint32_t percentage) = 0;
        virtual void showBatteryChargingDone()                  = 0;
        virtual void showBatteryCharging()                      = 0;

      public:
        BatteryBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        bool update(const Store::Battery &batteryContext);
    };
} // namespace gui::status_bar
