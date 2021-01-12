// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControl.hpp"

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>
#include <agents/settings/SystemSettings.hpp>

namespace screen_light_control
{
    namespace
    {
        auto from_string(const std::string &str, bool def) -> bool
        {
            try {
                return std::stoi(str) == 0;
            }
            catch (std::exception &) {
                return def;
            }
        }

        auto from_string(const std::string &str, float def) -> float
        {
            try {
                return std::stof(str);
            }
            catch (std::exception &) {
                return def;
            }
        }
    } // namespace

    ScreenLightControl::ScreenLightControl(sys::Service *parent)
        : settings(std::make_unique<settings::Settings>(parent))
    {
        controlTimer = std::make_unique<sys::Timer>("LightControlTimer", parent, CONTROL_TIMER_MS);
        readoutTimer = std::make_unique<sys::Timer>("LightSensorReadoutTimer", parent, READOUT_TIMER_MS);

        initFromSettings();
    }

    ScreenLightControl::~ScreenLightControl()
    {
        disableTimers();
    }

    void ScreenLightControl::initFromSettings()
    {
        settings->registerValueChange(settings::Brightness::brightnessLevel,
                                      [&](const std::string &value) { setBrightnessLevel(from_string(value, 0.0f)); });

        settings->registerValueChange(settings::Brightness::autoMode, [&](const std::string &value) {
            if (from_string(value, false)) {
                enableAutomaticMode();
            }
            else {
                disableAutomaticMode();
            }
        });

        settings->registerValueChange(settings::Brightness::autoMode, [&](const std::string &value) {
            if (from_string(value, false)) {
                turnOn();
            }
            else {
                turnOff();
            }
        });
    }

    void ScreenLightControl::processRequest(Action action, const Parameters &params)
    {
        switch (action) {
        case Action::turnOff:
            turnOff();
            setScreenLightSettings(settings::Brightness::state, lightOn);
            break;
        case Action::turnOn:
            turnOn();
            setScreenLightSettings(settings::Brightness::state, lightOn);
            break;
        case Action::enableAutomaticMode:
            enableAutomaticMode();
            setScreenLightSettings(settings::Brightness::autoMode, automaticMode);
            break;
        case Action::disableAutomaticMode:
            disableAutomaticMode();
            setScreenLightSettings(settings::Brightness::autoMode, automaticMode);
            break;
        case Action::setManualModeBrightness:
            setBrightnessLevel(params.manualModeBrightness);
            setScreenLightSettings(settings::Brightness::brightnessLevel, params.manualModeBrightness);
            break;
        case Action::setGammaCorrectionFactor:
            setGammaFactor(params.gammaFactor);
            break;
        case Action::setAutomaticModeParameters:
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
        controlTimer->connect([&](sys::Timer &) { controlTimerCallback(); });
        readoutTimer->connect([&](sys::Timer &) { readoutTimerCallback(); });
        controlTimer->start();
        readoutTimer->start();
    }

    void ScreenLightControl::disableTimers()
    {
        controlTimer->stop();
        readoutTimer->stop();
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
        if (automaticMode) {
            enableTimers();
        }
        lightOn = true;
    }

    void ScreenLightControl::setBrightnessLevel(bsp::eink_frontlight::BrightnessPercentage brightnessPercentage)
    {
        bsp::eink_frontlight::setBrightness(brightnessPercentage);
        brightnessValue = brightnessPercentage;
    }

    void ScreenLightControl::setGammaFactor(float gammaFactor)
    {
        bsp::eink_frontlight::setGammaFactor(gammaFactor);
        setScreenLightSettings(settings::Brightness::gammaFactor, gammaFactor);
    }

    void ScreenLightControl::turnOff()
    {
        bsp::eink_frontlight::turnOff();
        bsp::light_sensor::standby();
        controlTimer->stop();
        readoutTimer->stop();
        lightOn = false;
    }
} // namespace screen_light_control
