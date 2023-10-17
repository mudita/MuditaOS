// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BatteryBase.hpp"

namespace gui
{
    class Image;
}

namespace gui::status_bar
{
    class BatteryBar : public BatteryBase
    {
      public:
        BatteryBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

      private:
        void showBatteryLevel(std::uint32_t percentage) override;
        void showBatteryChargingDone() override;
        void showBatteryCharging() override;

        Image *img = nullptr;
    };
} // namespace gui::status_bar
