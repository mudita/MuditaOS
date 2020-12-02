// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp::light_sensor
{
    std::int32_t init();

    void deinit();

    bool standby();

    void wakeup();

    bool reset();

    bool isPresent();

    void readout();
} // namespace bsp::light_sensor
