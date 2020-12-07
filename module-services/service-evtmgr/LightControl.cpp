// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/LightControl.hpp"

namespace sevm::light_control
{
    namespace
    {
        constexpr inline auto CONTROL_TIMER_MS = 25;
        constexpr inline auto READOUT_TIMER_MS = 500;
        std::unique_ptr<sys::Timer> controlTimer;
        std::unique_ptr<sys::Timer> readoutTimer;

        bool automaticMode = false;
        typedef struct
        {
            float xBound;
            float a;
            float b;
        } FunctionSection;

        std::vector<FunctionSection> function;
        float rampStep;
        float brightnessHysteresis;

        float brightnessRampTarget;
        float brightnessRampState;
        bool rampTargetReached = false;

        float calculateBrightness(bsp::light_sensor::IlluminanceLux measurement)
        {
            for (const auto &section : function) {
                if (measurement < section.xBound) {
                    return section.a * measurement + section.b;
                }
            }

            if (function.empty()) {
                return 0.0f;
            }
            else {
                return function.back().xBound * function.back().a + function.back().b;
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
                        rampTargetReached   = true;
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

            return static_cast<bsp::eink_frontlight::BrightnessPercentage>(brightnessRampState);
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

            function.clear();
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
                    section.b = params.functionPoints[i - 1].second - section.a * params.functionPoints[i - 1].first;
                }
                function.push_back(section);
            }
        }
    } // namespace

    void init(sys::Service *parent)
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);
    }

    void deinit()
    {
        disableTimers();
    }

    bool processRequest(LightControlAction action, const Parameters &params)
    {
        switch (action) {
        case LightControlAction::turnOff:
            bsp::keypad_backlight::shutdown();
            bsp::eink_frontlight::turnOff();
            bsp::light_sensor::standby();
            controlTimer->stop();
            readoutTimer->stop();
            break;
        case LightControlAction::turnOn:
            bsp::keypad_backlight::turnOnAll();
            bsp::eink_frontlight::turnOn();
            bsp::light_sensor::wakeup();
            if (automaticMode) {
                enableTimers();
            }
            break;
        case LightControlAction::enableAutomaticMode:
            enableTimers();
            automaticMode = true;
            break;
        case LightControlAction::disableAutomaticMode:
            disableTimers();
            automaticMode = false;
            break;
        case LightControlAction::setManualModeBrightness:
            bsp::eink_frontlight::setBrightness(params.manualModeBrightness);
            break;
        case LightControlAction::setGammaCorrectionFactor:
            bsp::eink_frontlight::setGammaFactor(params.gammaFactor);
            break;
        case LightControlAction::setAutomaticModeParameters:
            setAutomaticModeParameters(params);
            break;
        }
        return true;
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

} // namespace sevm::light_control
