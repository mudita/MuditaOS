// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControl.hpp"

#include <Timers/TimerFactory.hpp>
#include <Service/Service.hpp>

namespace pure::screen_light_control
{
    namespace
    {
        constexpr bsp::eink_frontlight::BrightnessPercentage fadeOutBrigthnessMax = 35.0f;
    }

    ScreenLightController::ScreenLightController(sys::Service *parent)
    {
        controlTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightControlTimer",
                                                              std::chrono::milliseconds{CONTROL_TIMER_MS},
                                                              [this](sys::Timer &) { controlTimerCallback(); });
        readoutTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightSensorReadoutTimer",
                                                              std::chrono::milliseconds{READOUT_TIMER_MS},
                                                              [this](sys::Timer &) { readoutTimerCallback(); });

        setParameters(AutomaticModeParameters());
    }

    ScreenLightController::~ScreenLightController()
    {
        disableTimers();
    }

    void ScreenLightController::processRequest(Action action)
    {
        processRequest(action, Parameters());
    }

    void ScreenLightController::processRequest(Action action, const Parameters &params)
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
        case Action::fadeOut:
            handleFadeOut();
            break;
        default:
            break;
        }
    }

    void ScreenLightController::controlTimerCallback()
    {
        bsp::eink_frontlight::setBrightness(::screen_light_control::functions::brightnessRampOut());
    }

    void ScreenLightController::readoutTimerCallback()
    {
        if (!fadeOut) {
            ::screen_light_control::functions::calculateBrightness(bsp::light_sensor::readout());
        }
    }

    auto ScreenLightController::isLightOn() const noexcept -> bool
    {
        return lightOn;
    }

    bool ScreenLightController::isAutoModeOn() const noexcept
    {
        return automaticMode == ScreenLightMode::Automatic;
    }

    auto ScreenLightController::getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage
    {
        return brightnessValue;
    }

    void ScreenLightController::enableTimers()
    {
        controlTimer.start();
        readoutTimer.start();
    }

    void ScreenLightController::disableTimers()
    {
        controlTimer.stop();
        readoutTimer.stop();
    }

    void ScreenLightController::setParameters(const AutomaticModeParameters &params)
    {
        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            disableTimers();
        }

        ::screen_light_control::functions::setRampStep(
            100.0f * (static_cast<float>(CONTROL_TIMER_MS) / static_cast<float>(params.rampTimeMS)));
        ::screen_light_control::functions::setHysteresis(params.brightnessHysteresis);
        ::screen_light_control::functions::setFunctionFromPoints(params.functionPoints);

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
    }

    void ScreenLightController::setParameters(ManualModeParameters params)
    {
        brightnessValue = params.manualModeBrightness;
        setManualBrightnessLevel();
    }

    void ScreenLightController::enableAutomaticMode()
    {
        if (lightOn) {
            enableTimers();
        }
        automaticMode = ScreenLightMode::Automatic;
    }

    void ScreenLightController::disableAutomaticMode()
    {
        disableTimers();
        automaticMode = ScreenLightMode::Manual;
        setManualBrightnessLevel();
    }

    void ScreenLightController::turnOn()
    {
        fadeOut = false;

        bsp::eink_frontlight::turnOn();
        bsp::light_sensor::wakeup();

        if (automaticMode == ScreenLightMode::Automatic) {
            if (lightOn) {
                ::screen_light_control::functions::calculateBrightness(bsp::light_sensor::readout());
            }
            else {
                // It takes some time to get initial readout -> using last saved measurement
                ::screen_light_control::functions::calculateBrightness(stashedReadout);
            }
            ::screen_light_control::functions::resetRampToTarget();
            bsp::eink_frontlight::setBrightness(::screen_light_control::functions::getRampState());
            enableTimers();
        }
        lightOn = true;
    }

    void ScreenLightController::setManualBrightnessLevel()
    {
        bsp::eink_frontlight::setBrightness(brightnessValue);
    }

    void ScreenLightController::turnOff()
    {
        bsp::eink_frontlight::turnOff();
        stashedReadout = bsp::light_sensor::readout();
        disableTimers();
        bsp::light_sensor::standby();
        lightOn = false;
        fadeOut = false;
    }

    void ScreenLightController::handleFadeOut()
    {
        if (automaticMode == ScreenLightMode::Automatic) {
            fadeOut = true;
            // Set fadeout brightess as maximum or current ramp state if lower
            auto rampState         = ::screen_light_control::functions::getRampState();
            auto fadeOutBrigthness = std::clamp(rampState, 0.0f, fadeOutBrigthnessMax);
            ::screen_light_control::functions::setRampTarget(fadeOutBrigthness);
        }
    }

    bool ScreenLightController::isFadeOutOngoing()
    {
        return fadeOut;
    }

} // namespace pure::screen_light_control
