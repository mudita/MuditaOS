// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "evtmgr/temperature-compensation/TemperatureProvider.hpp"
#include "internal/StaticData.hpp"

#include <log/log.hpp>
#include <hal/temperature_source/TemperatureSource.hpp>

#include <cmath>

void TemperatureProvider::updateTemperature(hal::temperature::AbstractTemperatureSource &source)
{
    const auto temp = source.read();
    if (not temp) {
        LOG_FATAL("Error during reading from temperature source");
    }
    else {
        updateCompensation();
        float rawReadout  = *temp;
        float temperature = rawReadout - staticTemperatureOffset - currentCompensationValue;
        LOG_INFO("---------->>>>>>> Temps %d %d", int(rawReadout * 100), int(currentCompensationValue * 100));
        evtmgr::internal::StaticData::get().setCurrentTemperature(temperature);
    }
}

void TemperatureProvider::updateCompensation()
{
    auto diff =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - audioLastTimeStateChange);
    auto secondsElapsed = diff.count();
    auto decay          = std::exp(-secondsElapsed / audioCompensationTimeCoefficient);
    if (audioPlaying) {
        currentCompensationValue += (audioCompensationAmplitude - currentCompensationValue) * (1 - decay);
    }
    else {
        currentCompensationValue = (currentCompensationValue)*decay;
    }
    audioLastTimeStateChange = std::chrono::system_clock::now();
}

void TemperatureProvider::audioStarted()
{
    if (audioPlaying == false) {
        updateCompensation();
        audioPlaying = true;
    }
}

void TemperatureProvider::audioStoped()
{
    if (audioPlaying == true) {
        updateCompensation();
        audioPlaying = false;
    }
}
