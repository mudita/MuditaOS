// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <service-db/Settings.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace battery_level_check
{
    void init(sys::Service *service, std::shared_ptr<settings::Settings> &setts);

    void deinit();

    void checkBatteryLevel();

    void setBatteryCriticalLevel(unsigned int level);
} // namespace battery_level_check
