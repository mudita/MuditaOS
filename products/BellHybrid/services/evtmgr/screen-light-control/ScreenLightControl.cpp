// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>
#include <module-sys/Timers/TimerFactory.hpp>
#include <Service/Service.hpp>

namespace screen_light_control
{
    ScreenLightControl::ScreenLightControl(sys::Service *parent)
    {
        controlTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightControlTimer",
                                                              std::chrono::milliseconds{CONTROL_TIMER_MS},
                                                              [this](sys::Timer &) { controlTimerCallback(); });
        readoutTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightSensorReadoutTimer",
                                                              std::chrono::milliseconds{READOUT_TIMER_MS},
                                                              [this](sys::Timer &) { readoutTimerCallback(); });

        setParameters(screen_light_control::AutomaticModeParameters());
    }

    ScreenLightControl::~ScreenLightControl()
    {
        disableTimers();
    }

    void ScreenLightControl::processRequest(Action action)
    {
        processRequest(action, Parameters());
    }
    void ScreenLightControl::processRequest(Action action, const Parameters &params)
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
            if (params.hasManualModeParams()) {
                setParameters(params.getManualModeParams());
            }
            break;
        case Action::setAutomaticModeParameters:
            if (params.hasAutoModeParams()) {
                setParameters(params.getAutoModeParams());
            }
            break;
        }
    }

    void ScreenLightControl::controlTimerCallback()
    {
        bsp::eink_frontlight::setBrightness(functions::brightnessRampOut());
    }

    void ScreenLightControl::readoutTimerCallback()
    {
        functions::calculateBrightness(brightnessValue);
    }

    auto ScreenLightControl::getAutoModeState() const noexcept -> ScreenLightMode
    {
        return automaticMode;
    }

    auto ScreenLightControl::isLightOn() const noexcept -> bool
    {
        return lightOn;
    }

    auto ScreenLightControl::getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return brightnessValue;
    }

    void ScreenLightControl::enableTimers()
    {
        controlTimer.start();
        readoutTimer.start();
    }

    void ScreenLightControl::disableTimers()
    {
        controlTimer.stop();
        readoutTimer.stop();
    }

    void ScreenLightControl::setParameters(const AutomaticModeParameters &params)
    {
        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            disableTimers();
        }

        functions::setRampStep(100.0f * (static_cast<float>(CONTROL_TIMER_MS) / static_cast<float>(params.rampTimeMS)));
        functions::setHysteresis(params.brightnessHysteresis);
        functions::setFunctionFromPoints(params.functionPoints);

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
    }

    void ScreenLightControl::setParameters(ManualModeParameters params)
    {
        brightnessValue = params.manualModeBrightness;
        setManualBrightnessLevel();
    }

    void ScreenLightControl::enableAutomaticMode()
    {
        if (lightOn) {
            enableTimers();
        }
        automaticMode = ScreenLightMode::Automatic;
    }

    void ScreenLightControl::disableAutomaticMode()
    {
        disableTimers();
        automaticMode = ScreenLightMode::Manual;
        setManualBrightnessLevel();
    }

    void ScreenLightControl::turnOn()
    {
        bsp::eink_frontlight::turnOn();
        if (automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
        lightOn = true;
    }

    void ScreenLightControl::setManualBrightnessLevel()
    {
        bsp::eink_frontlight::setBrightness(brightnessValue);
    }

    void ScreenLightControl::turnOff()
    {
        bsp::eink_frontlight::turnOff();
        disableTimers();
        lightOn = false;
    }

} // namespace screen_light_control
