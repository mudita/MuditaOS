// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "internal/StaticData.hpp"

#include <evtmgr/EventManager.hpp>
#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>

namespace
{
    auto updateTemperature = [](hal::temperature::AbstractTemperatureSource &source) {
        const auto temp = source.read();
        if (not temp) {
            LOG_FATAL("Error during reading from temperature source");
        }
        else {
            evtmgr::internal::StaticData::get().setCurrentTemperature(*temp);
        };
    };
}

EventManager::EventManager(const std::string &name)
    : EventManagerCommon(name), temperatureSource{hal::temperature::AbstractTemperatureSource::Factory::create()}
{
    updateTemperature(*temperatureSource);

    onMinuteTick = [this](const time_t) { updateTemperature(*temperatureSource); };
}
