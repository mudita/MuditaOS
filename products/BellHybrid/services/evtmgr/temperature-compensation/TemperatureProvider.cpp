// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "evtmgr/temperature-compensation/TemperatureProvider.hpp"
#include "internal/StaticData.hpp"

#include <log/log.hpp>
#include <hal/temperature_source/TemperatureSource.hpp>
#include <board/rt1051/puretx/hal/battery_charger/BatteryCharger.hpp>

#include <cmath>

void TemperatureProvider::updateTemperature(hal::temperature::AbstractTemperatureSource &source)
{
    const auto temp = source.read();
    if (not temp) {
        LOG_FATAL("Error during reading from temperature source");
    }
    else {
        float currentCompensationValue = getCompensationValue();
        float rawReadout               = *temp;
        float temperature              = rawReadout - currentCompensationValue;
        LOG_INFO("Temperature compensation: raw- %d audio %d charging %d",
                 int(rawReadout * 100),
                 int(audioCompensationValue * 100),
                 int(chargingCompensationValue * 100));
        evtmgr::internal::StaticData::get().setCurrentTemperature(temperature);
    }
}

float TemperatureProvider::getCompensationValue()
{
    updateAudioCompensation();
    updateChargingCompensation();
    return staticTemperatureOffset + audioCompensationValue + chargingCompensationValue;
}

void TemperatureProvider::updateAudioCompensation()
{
    auto diff =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - audioLastTimeStateChange);
    auto secondsElapsed = diff.count();
    auto decay          = std::exp(-secondsElapsed / audioCompensationTimeCoefficient);
    if (audioPlaying) {
        audioCompensationValue += (audioCompensationAmplitude - audioCompensationValue) * (1 - decay);
    }
    else {
        audioCompensationValue = (audioCompensationValue)*decay;
    }
    audioLastTimeStateChange = std::chrono::system_clock::now();
}

void TemperatureProvider::updateChargingCompensation()
{
    auto diff           = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() -
                                                                 chargingLastTimeStateChange);
    auto secondsElapsed = diff.count();
    auto decay          = std::exp(-secondsElapsed / chargingCompensationTimeCoefficient);
    if (battery.get().state == Store::Battery::State::Charging) {
        chargingCompensationValue += (chargingCompensationAmplitude - chargingCompensationValue) * (1 - decay);
    }
    else {
        chargingCompensationValue = (chargingCompensationValue)*decay;
    }
    chargingLastTimeStateChange = std::chrono::system_clock::now();
}

void TemperatureProvider::audioStarted()
{
    if (audioPlaying == false) {
        updateAudioCompensation();
        audioPlaying = true;
    }
}

void TemperatureProvider::audioStoped()
{
    if (audioPlaying == true) {
        updateAudioCompensation();
        audioPlaying = false;
    }
}
