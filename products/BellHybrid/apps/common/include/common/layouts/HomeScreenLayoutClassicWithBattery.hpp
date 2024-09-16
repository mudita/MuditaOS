// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithBattery : public HomeScreenLayoutClassic
    {
      public:
        explicit HomeScreenLayoutClassicWithBattery(std::string name);

      protected:
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;
    };
} // namespace gui
