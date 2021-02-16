// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class HBox;

    class BatteryWidgetBase : public HBox
    {
        virtual void showBatteryLevel(std::uint32_t percentage) = 0;
        virtual void showBatteryPluggedNotCharging()            = 0;
        virtual void showBatteryCharging()                      = 0;

      public:
        BatteryWidgetBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void show(const Store::Battery batteryContext, bool shown);
    };
} // namespace gui
