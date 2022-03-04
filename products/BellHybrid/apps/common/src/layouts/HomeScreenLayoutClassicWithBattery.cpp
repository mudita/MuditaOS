// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/layouts/HomeScreenLayoutClassicWithBattery.hpp>
#include <common/widgets/BellBattery.hpp>

namespace gui
{
    HomeScreenLayoutClassicWithBattery::HomeScreenLayoutClassicWithBattery(std::string name)
        : HomeScreenLayoutClassic(std::move(name))
    {}

    bool HomeScreenLayoutClassicWithBattery::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return true;
    }
}; // namespace gui
