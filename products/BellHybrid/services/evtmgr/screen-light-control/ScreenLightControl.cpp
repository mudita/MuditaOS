// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLightControl.hpp"

#include <Service/Service.hpp>
#include <Timers/TimerFactory.hpp>
#include <Utils.hpp>

#include <system/Constants.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace bell::screen_light_control
{
    ScreenLightController::ScreenLightController(sys::Service *parent)
        : cpuSentinel(std::make_shared<sys::CpuSentinel>(screenLightControlName, parent))
    {
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        parent->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

        controlTimer  = sys::TimerFactory::createPeriodicTimer(parent,
                                                              "LightControlTimer",
                                                              std::chrono::milliseconds{CONTROL_TIMER_MS},
                                                              [this](sys::Timer &) { controlTimerCallback(); });
        automaticMode = ScreenLightMode::Automatic;
    }

    ScreenLightController::~ScreenLightController()
    {
        disableTimers();
    }

    void ScreenLightController::processRequest(Action action)
    {
        processRequest(action, Parameters{});
    }

    void ScreenLightController::processRequest(Action action, const Parameters &params)
    {
        switch (action) {
        case Action::turnOff:
            turnOff();
            break;
        case Action::turnOn:
            turnOn(params.hasManualModeParams() ? std::optional<ManualModeParameters>(params.getManualModeParams())
                                                : std::nullopt);
            break;
        case Action::setAutomaticModeParameters:
            if (params.hasLinearProgressModeParams()) {
                setParameters(params.getLinearProgressModeParams());
            }
            else if (params.hasConstLinearProgressModeParams()) {
                setParameters(params.getConstLinearProgressModeParams());
            }
            break;
        default:
            break;
        }
    }

    void ScreenLightController::controlTimerCallback()
    {
        auto newBrightness = ::screen_light_control::functions::brightnessRampOut();
        bsp::eink_frontlight::setBrightness(newBrightness);

        if (::screen_light_control::functions::isRampTargetReached()) {
            if (!automaticModeFunctions.empty()) {
                setUpAutomaticFunction(getNextAutomaticFunction(), newBrightness);
            }
            else {
                if (newBrightness == MINIMAL_TARGET) {
                    turnOff();
                    bsp::eink_frontlight::turnOff();
                }
                setBrightnessInstant(MINIMAL_TARGET);
                disableTimers();
            }
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
        return brightnessWhenOn;
    }

    void ScreenLightController::enableTimers()
    {
        controlTimer.start();
    }

    void ScreenLightController::disableTimers()
    {
        controlTimer.stop();
    }

    void ScreenLightController::setParameters(const LinearProgressModeParameters &params)
    {
        if (params.functions.empty()) {
            disableTimers();
            return;
        }
        setAutomaticModeFunctions(params.functions);

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            disableTimers();
        }

        ::screen_light_control::functions::setHysteresis(params.brightnessHysteresis);
        setUpAutomaticFunction(getNextAutomaticFunction(), ::screen_light_control::functions::getRampState());
        setBrightnessInstant(getStandarizedRampTarget(params.startBrightnessValue));

        if (lightOn && automaticMode == ScreenLightMode::Automatic) {
            enableTimers();
        }
    }

    void ScreenLightController::setAutomaticModeFunctions(
        const LinearProgressModeParameters::LinearFunctions &functions)
    {
        automaticModeFunctions = LinearProgressModeParameters::LinearFunctions{functions.rbegin(), functions.rend()};
    }

    ::screen_light_control::functions::LinearProgressFunction ScreenLightController::getNextAutomaticFunction()
    {
        auto function = automaticModeFunctions.back();
        automaticModeFunctions.pop_back();
        return function;
    }

    void ScreenLightController::setUpAutomaticFunction(
        ::screen_light_control::functions::LinearProgressFunction function,
        bsp::eink_frontlight::BrightnessPercentage currentBrightness)
    {
        const auto rampTarget = getStandarizedRampTarget(function.target);
        ::screen_light_control::functions::setRampStep(
            std::abs(rampTarget - currentBrightness) /
            (static_cast<float>(function.duration.count()) / CONTROL_TIMER_MS));
        ::screen_light_control::functions::setRampTarget(rampTarget);
    }

    void ScreenLightController::setParameters(const ConstLinearProgressModeParameters &params)
    {
        setBrightnessInstant(getStandarizedRampTarget(params.targetBrightness));
    }

    void ScreenLightController::turnOn(const std::optional<ManualModeParameters> &params)
    {
        if (params.has_value()) {
            brightnessWhenOn = getStandarizedRampTarget(params->manualModeBrightness);
        }

        ::screen_light_control::functions::setRampTarget(brightnessWhenOn);
        bsp::eink_frontlight::turnOn();
        cpuSentinelKeepOn();
        enableTimers();
        lightOn = true;
    }

    void ScreenLightController::setBrightnessInstant(bsp::eink_frontlight::BrightnessPercentage brightness)
    {
        brightnessWhenOn = getStandarizedRampTarget(brightness);
        ::screen_light_control::functions::setRampTarget(brightnessWhenOn);
        ::screen_light_control::functions::setRampStep(RAMP_STEP_PER_MS * CONTROL_TIMER_MS);
    }

    void ScreenLightController::turnOff()
    {
        lightOn = false;
        ::screen_light_control::functions::setRampTarget(MINIMAL_TARGET);
        cpuSentinelRelease();
        enableTimers();
    }

    void ScreenLightController::cpuSentinelKeepOn()
    {
        cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
    }

    void ScreenLightController::cpuSentinelRelease()
    {
        cpuSentinel->ReleaseMinimumFrequency();
    }

    bsp::eink_frontlight::BrightnessPercentage ScreenLightController::getStandarizedRampTarget(
        bsp::eink_frontlight::BrightnessPercentage target)
    {
        return std::max<bsp::eink_frontlight::BrightnessPercentage>(target, MINIMAL_TARGET);
    }

    bool ScreenLightController::isFadeOutOngoing()
    {
        return false;
    }

} // namespace bell::screen_light_control
