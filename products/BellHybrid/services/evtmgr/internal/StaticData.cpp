// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "StaticData.hpp"

namespace evtmgr::internal
{
    void StaticData::setCurrentTemperature(hal::temperature::AbstractTemperatureSource::Temperature temperature)
    {
        currentTemperature = temperature;
    }

    hal::temperature::AbstractTemperatureSource::Temperature StaticData::getCurrentTemperature() const
    {
        return currentTemperature;
    }

    StaticData &StaticData::get()
    {
        static auto instance = StaticData{};
        return instance;
    }
} // namespace evtmgr::internal
