// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/temperature_source/TemperatureSource.hpp>
#include <module-utils/EventStore/EventStore.hpp>

#include <chrono>

class TemperatureProvider
{
    std::chrono::time_point<std::chrono::system_clock> audioLastTimeStateChange    = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> chargingLastTimeStateChange = std::chrono::system_clock::now();

    bool audioPlaying                      = false;
    float audioCompensationValue           = 0;
    float audioCompensationTimeCoefficient = 750;
    float audioCompensationAmplitude       = 2.75;

    float chargingCompensationValue           = 0;
    float chargingCompensationTimeCoefficient = 750;
    float chargingCompensationAmplitude       = 7;

    float staticTemperatureOffset = 1.75;

    Store::Battery battery;

  public:
    void updateTemperature(hal::temperature::AbstractTemperatureSource &source);
    auto getCompensationValue() -> float;
    void updateAudioCompensation();
    void updateChargingCompensation();
    void audioStarted();
    void audioStoped();
};