// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/temperature_source/TemperatureSource.hpp>

#include <chrono>

class TemperatureProvider
{
    std::chrono::time_point<std::chrono::system_clock> audioLastTimeStateChange = std::chrono::system_clock::now();
    bool audioPlaying                                                           = false;
    float compensationValue                                                     = 0;
    float audioCompensationTimeCoefficient                                      = 750;
    float audioCompensationAmplitude                                            = 2.75;
    float currentCompensationValue                                              = 0;
    float staticTemperatureOffset                                               = 1.75;

  public:
    void updateTemperature(hal::temperature::AbstractTemperatureSource &source);
    void updateCompensation();
    void audioStarted();
    void audioStoped();
};