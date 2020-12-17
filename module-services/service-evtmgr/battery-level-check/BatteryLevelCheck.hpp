// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace battery_level_check
{
    void init(sys::Service *service);

    void checkBatteryLevelCritical();

    void setBatteryCriticalLevel(unsigned int level);
} // namespace battery_level_check
