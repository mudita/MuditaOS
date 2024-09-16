// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BatteryLevelData.hpp"

namespace bluetooth
{
    BatteryLevelData::BatteryLevelData(const BatteryLevel &level) : level(level)
    {}
    auto BatteryLevelData::getData() -> DataVariant
    {
        return level;
    }
} // namespace bluetooth
