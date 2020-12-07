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

        typedef struct
        {
            float xBound;
            float a;
            float b;
        } FunctionSection;

        Parameters parameters;
        std::vector<FunctionSection> function;
        float rampStep = 0.0f;

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
            if (rampTargetReached &&
                std::abs(brightnessRampTarget - brightnessRampState) > parameters.brightnessHysteresis) {
                rampTargetReached = false;
                LOG_DEBUG("out of hysteresis");
            }

            if (!rampTargetReached) {
                if (brightnessRampState < brightnessRampTarget) {
                    brightnessRampState += rampStep;
                    if (brightnessRampState > brightnessRampTarget) {
                        rampTargetReached   = true;
                        LOG_DEBUG("ramp taget reached");
                    }
                }
                else if (brightnessRampState > brightnessRampTarget) {
                    brightnessRampState -= rampStep;
                    if (brightnessRampState < brightnessRampTarget) {
                        brightnessRampState = brightnessRampTarget;
                        rampTargetReached   = true;
                        LOG_DEBUG("ramp taget reached");
                    }
                }
            }

            return static_cast<bsp::eink_frontlight::BrightnessPercentage>(brightnessRampState);
        }

        void setParameters(const Parameters &params)
        {
            parameters = params;
            rampStep   = 100.0f * (static_cast<float>(CONTROL_TIMER_MS) / static_cast<float>(parameters.rampTimeMS));
            bsp::eink_frontlight::setGammaFactor(parameters.gammaFactor);

            function.clear();
            for (unsigned int i = 0; i < parameters.functionPoints.size(); ++i) {
                FunctionSection section;
                section.xBound = parameters.functionPoints[i].first;
                if (i == 0) {
                    section.a = 0.0f;
                    section.b = parameters.functionPoints[i].second;
                }
                else {
                    section.a = (parameters.functionPoints[i - 1].second - parameters.functionPoints[i].second) /
                                (parameters.functionPoints[i - 1].first - parameters.functionPoints[i].first);
                    section.b =
                        parameters.functionPoints[i - 1].second - section.a * parameters.functionPoints[i - 1].first;
                }
                function.push_back(section);
            }
        }
    } // namespace

    void init(sys::Service *parent)
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        controlTimer->connect([&](sys::Timer &) { controlTimerCallback(); });

        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);
        readoutTimer->connect([&](sys::Timer &) { readoutTimerCallback(); });
    }

    void deinit()
    {
        controlTimer->stop();
        readoutTimer->stop();
    }

    bool processRequest(LightControlAction action, const Parameters &params)
    {
        switch (action) {
        case (LightControlAction::turnOff):
            bsp::keypad_backlight::shutdown();
            bsp::eink_frontlight::turnOff();
            bsp::light_sensor::standby();
            break;
        case (LightControlAction::turnOn):
            bsp::keypad_backlight::turnOnAll();
            bsp::eink_frontlight::turnOn();
            bsp::light_sensor::wakeup();
            break;
        case (LightControlAction::enableAutomaticMode):
            controlTimer->start();
            readoutTimer->start();
            break;
        case (LightControlAction::disableAutomaticMode):
            controlTimer->stop();
            readoutTimer->stop();
            break;
        }

        setParameters(params);

        return true;
    }

    void controlTimerCallback()
    {
        auto out = brightnessRampOut();
        // LOG_DEBUG("ramp out: %d", out);
        bsp::eink_frontlight::setBrightness(out);
    }

    void readoutTimerCallback()
    {
        float lightMeasurement = bsp::light_sensor::readout();
        brightnessRampTarget   = calculateBrightness(lightMeasurement);
        LOG_DEBUG(
            "measurement: %d , bright: %d", static_cast<int>(lightMeasurement), static_cast<int>(brightnessRampTarget));
    }

} // namespace sevm::light_control
