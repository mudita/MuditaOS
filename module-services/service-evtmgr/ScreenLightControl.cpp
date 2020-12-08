// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/ScreenLightControl.hpp"

namespace sevm::screen_light_control
{
    namespace
    {
        constexpr inline auto CONTROL_TIMER_MS = 25;
        constexpr inline auto READOUT_TIMER_MS = 500;
        std::unique_ptr<sys::Timer> controlTimer;
        std::unique_ptr<sys::Timer> readoutTimer;

        bool automaticMode = false;
        bool lightOn       = false;
        typedef struct
        {
            float xBound;
            float a;
            float b;
        } FunctionSection;

        std::vector<FunctionSection> brightnessFunction;
        float rampStep;
        float brightnessHysteresis;

        bsp::eink_frontlight::BrightnessPercentage brightnessRampTarget;
        bsp::eink_frontlight::BrightnessPercentage brightnessRampState;
        bool rampTargetReached = false;

        float calculateBrightness(bsp::light_sensor::IlluminanceLux measurement)
        {
            for (const auto &section : brightnessFunction) {
                if (measurement < section.xBound) {
                    return section.a * measurement + section.b;
                }
            }

            if (brightnessFunction.empty()) {
                return 0.0f;
            }
            else {
                return brightnessFunction.back().xBound * brightnessFunction.back().a + brightnessFunction.back().b;
            }
        }

        bsp::eink_frontlight::BrightnessPercentage brightnessRampOut()
        {
            if (rampTargetReached && std::abs(brightnessRampTarget - brightnessRampState) > brightnessHysteresis) {
                rampTargetReached = false;
            }

            if (!rampTargetReached) {
                if (brightnessRampState < brightnessRampTarget) {
                    brightnessRampState += rampStep;
                    if (brightnessRampState > brightnessRampTarget) {
                        rampTargetReached = true;
                    }
                }
                else if (brightnessRampState > brightnessRampTarget) {
                    brightnessRampState -= rampStep;
                    if (brightnessRampState < brightnessRampTarget) {
                        brightnessRampState = brightnessRampTarget;
                        rampTargetReached   = true;
                    }
                }
            }

            return brightnessRampState;
        }

        void controlTimerCallback()
        {
            auto out = brightnessRampOut();
            bsp::eink_frontlight::setBrightness(out);
        }

        void readoutTimerCallback()
        {
            float lightMeasurement = bsp::light_sensor::readout();
            brightnessRampTarget   = calculateBrightness(lightMeasurement);
        }

        void enableTimers()
        {
            controlTimer->connect([&](sys::Timer &) { controlTimerCallback(); });
            readoutTimer->connect([&](sys::Timer &) { readoutTimerCallback(); });
            controlTimer->start();
            readoutTimer->start();
        }

        void disableTimers()
        {
            controlTimer->stop();
            readoutTimer->stop();
        }

        void setAutomaticModeParameters(const Parameters &params)
        {
            rampStep = 100.0f * (static_cast<float>(CONTROL_TIMER_MS) / static_cast<float>(params.rampTimeMS));
            brightnessHysteresis = params.brightnessHysteresis;

            if (!params.functionPoints.empty()) {
                brightnessFunction.clear();
                for (unsigned int i = 0; i < params.functionPoints.size(); ++i) {
                    FunctionSection section;
                    section.xBound = params.functionPoints[i].first;
                    if (i == 0) {
                        section.a = 0.0f;
                        section.b = params.functionPoints[i].second;
                    }
                    else {
                        section.a = (params.functionPoints[i - 1].second - params.functionPoints[i].second) /
                                    (params.functionPoints[i - 1].first - params.functionPoints[i].first);
                        section.b =
                            params.functionPoints[i - 1].second - section.a * params.functionPoints[i - 1].first;
                    }
                    brightnessFunction.push_back(section);
                }
            }
        }
    } // namespace

    void init(sys::Service *parent)
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);

        Parameters params;
        setAutomaticModeParameters(params);
    }

    void deinit()
    {
        disableTimers();
    }

    void processRequest(Action action, const Parameters &params)
    {
        switch (action) {
        case Action::turnOff:
            bsp::eink_frontlight::turnOff();
            bsp::light_sensor::standby();
            controlTimer->stop();
            readoutTimer->stop();
            lightOn = false;
            break;
        case Action::turnOn:
            bsp::eink_frontlight::turnOn();
            bsp::light_sensor::wakeup();
            if (automaticMode) {
                enableTimers();
            }
            lightOn = true;
            break;
        case Action::enableAutomaticMode:
            if (lightOn) {
                enableTimers();
            }
            automaticMode = true;
            break;
        case Action::disableAutomaticMode:
            disableTimers();
            automaticMode = false;
            break;
        case Action::setManualModeBrightness:
            bsp::eink_frontlight::setBrightness(params.manualModeBrightness);
            break;
        case Action::setGammaCorrectionFactor:
            bsp::eink_frontlight::setGammaFactor(params.gammaFactor);
            break;
        case Action::setAutomaticModeParameters:
            if (lightOn && automaticMode) {
                disableTimers();
            }
            setAutomaticModeParameters(params);
            if (lightOn && automaticMode) {
                enableTimers();
            }
            break;
        }
    }

} // namespace sevm::screen_light_control
