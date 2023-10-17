// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <battery/BatteryState.hpp>
#include <battery/BatteryBrownoutDetector.hpp>

struct EventManagerParams
{
    BatteryState::Thresholds battery;
    BatteryBrownoutDetector::Thresholds voltage;
};
