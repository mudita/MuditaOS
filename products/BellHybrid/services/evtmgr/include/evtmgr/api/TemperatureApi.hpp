// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>

namespace evtmgr::api
{
    hal::temperature::AbstractTemperatureSource::Temperature getCurrentTemperature();
} // namespace evtmgr::api
