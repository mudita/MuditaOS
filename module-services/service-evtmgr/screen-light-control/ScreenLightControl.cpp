// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControl.hpp"

#include <agents/settings/SystemSettings.hpp>
#include <module-sys/Timers/TimerFactory.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <service-db/service-db/Settings.hpp>
#include <Utils.hpp>

namespace screen_light_control
{

    ScreenLightControl::ScreenLightControl(std::shared_ptr<settings::Settings> settings, sys::Service *parent)
        : settings(settings)
    {
        controlTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightControlTimer",
                                                              std::chrono::milliseconds{CONTROL_TIMER_MS},
                                                              [this](sys::Timer &) { controlTimerCallback(); });
        readoutTimer = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightSensorReadoutTimer",
                                                              std::chrono::milliseconds{READOUT_TIMER_MS},
                                                              [this](sys::Timer &) { readoutTimerCallback(); });
        initFromSettings();
    }

    ScreenLightControl::~ScreenLightControl()
    {
        disableTimers();
    }

    void ScreenLightControl::initFromSettings()
    {
        settings->registerValueChange(settings::Brightness::brightnessLevel, [&](const std::string &value) {
            setBrightnessLevel(utils::getNumericValue<float>(value));
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&](const std::string &value) {
            if (utils::getNumericValue<bool>(value)) {
                enableAutomaticMode();
            }
            else {
                disableAutomaticMode();
            }
        });

        settings->registerValueChange(settings::Brightness::state, [&](const std::string &value) {
            if (utils::getNumericValue<bool>(value)) {
                turnOn();
            }
            else {
                turnOff();
            }
        });
    }

    void ScreenLightControl::processRequest(Action action)
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
        }
    }

    void ScreenLightControl::processRequest(ParameterizedAction action, Parameters params)
    {
        switch (action) {
        case ParameterizedAction::setManualModeBrightness:
            setBrightnessLevel(params.manualModeBrightness);
            break;
        case ParameterizedAction::setAutomaticModeParameters:
            setAutomaticModeParameters(params);
            break;
        }
    }

    void ScreenLightControl::controlTimerCallback()
    {
        bsp::eink_frontlight::setBrightness(functions::brightnessRampOut());
    }

    void ScreenLightControl::readoutTimerCallback()
    {
        functions::calculateBrightness(bsp::light_sensor::readout());
    }

    auto ScreenLightControl::getAutoModeState() const noexcept -> ScreenLightMode
    {
        return automaticMode;
    }

    auto ScreenLightControl::getLightState() const noexcept -> bool
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

    void ScreenLightControl::setAutomaticModeParameters(const Parameters &params)
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
    }

    void ScreenLightControl::turnOn()
    {
        bsp::eink_frontlight::turnOn();
        bsp::light_sensor::wakeup();
        if (automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
        lightOn = true;
    }

    void ScreenLightControl::setBrightnessLevel(bsp::eink_frontlight::BrightnessPercentage brightnessPercentage)
    {
        bsp::eink_frontlight::setBrightness(brightnessPercentage);
        brightnessValue = brightnessPercentage;
    }

    void ScreenLightControl::turnOff()
    {
        bsp::eink_frontlight::turnOff();
        bsp::light_sensor::standby();
        controlTimer.stop();
        readoutTimer.stop();
        lightOn = false;
    }
} // namespace screen_light_control
