// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Image.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class BatteryWidget : public Image
    {
      public:
        BatteryWidget(Item *parent, uint32_t x, uint32_t y);
        void show(const Store::Battery batteryContext, bool shown);

      private:
        void showBatteryBars(std::uint32_t percentage);
    };
} // namespace gui
