// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
} // namespace gui
