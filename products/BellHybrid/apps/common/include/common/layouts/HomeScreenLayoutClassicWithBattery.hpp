// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithBattery : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithBattery(std::string name);

      protected:
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;
    };
}; // namespace gui
