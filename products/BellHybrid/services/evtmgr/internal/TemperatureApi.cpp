// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "internal/StaticData.hpp"

#include <evtmgr/api/TemperatureApi.hpp>

namespace evtmgr::api
{
    hal::temperature::AbstractTemperatureSource::Temperature getCurrentTemperature()
    {
        return internal::StaticData::get().getCurrentTemperature();
    }
} // namespace evtmgr::api
