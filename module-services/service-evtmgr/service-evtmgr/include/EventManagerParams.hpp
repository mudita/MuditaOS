// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <battery/BatteryState.hpp>
#include <battery/BatteryBrownoutDetector.hpp>

struct EventManagerParams
{
    BatteryState::Thresholds battery;
    BatteryBrownoutDetector::Thresholds voltage;
};
