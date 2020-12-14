// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControl.hpp"

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
            if (lightOn && automaticMode) {
                disableTimers();
            }

            functions::setRampStep(100.0f *
                                   (static_cast<float>(CONTROL_TIMER_MS) / static_cast<float>(params.rampTimeMS)));
            functions::setHysteresis(params.brightnessHysteresis);
            functions::setFunctionFromPoints(params.functionPoints);

            if (lightOn && automaticMode) {
                enableTimers();
            }
        }

        void turnOff()
        {
            bsp::eink_frontlight::turnOff();
            bsp::light_sensor::standby();
            controlTimer->stop();
            readoutTimer->stop();
            lightOn = false;
        }

        void turnOn()
        {
            bsp::eink_frontlight::turnOn();
            bsp::light_sensor::wakeup();
            if (automaticMode) {
                enableTimers();
            }
            lightOn = true;
        }

        void enableAutomaticMode()
        {
            if (lightOn) {
                enableTimers();
            }
            automaticMode = true;
        }

        void disableAutomaticMode()
        {
            disableTimers();
            automaticMode = false;
        }

    } // namespace

    void init(sys::Service *parent)
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);

        Parameters defaultParams;
        setAutomaticModeParameters(defaultParams);
    }

    void deinit()
    {
        disableTimers();
        controlTimer.reset();
        readoutTimer.reset();
    }

    void processRequest(Action action, const Parameters &params)
    {
        switch (action) {
        case Action::turnOff:
            turnOff();
            break;
        case Action::turnOn:
            turnOn();
            break;
        case Action::enableAutomaticMode:
            enableAutomaticMode();
            break;
        case Action::disableAutomaticMode:
            disableAutomaticMode();
            break;
        case Action::setManualModeBrightness:
            bsp::eink_frontlight::setBrightness(params.manualModeBrightness);
            break;
        case Action::setGammaCorrectionFactor:
            bsp::eink_frontlight::setGammaFactor(params.gammaFactor);
            break;
        case Action::setAutomaticModeParameters:
            setAutomaticModeParameters(params);
            break;
        }
    }

    void controlTimerCallback()
    {
        bsp::eink_frontlight::setBrightness(functions::brightnessRampOut());
    }

    void readoutTimerCallback()
    {
        functions::calculateBrightness(bsp::light_sensor::readout());
    }

} // namespace sevm::screen_light_control
