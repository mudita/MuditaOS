// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <Timers/TimerHandle.hpp>
#include <memory>

namespace sys
{
    class Service;
} // namespace sys

/// Screen light control algorithm. Automatic/Manual mode of operation.
/// Processing of ambient light sensor input to screen brightness output.
namespace bell::screen_light_control
{
    inline constexpr auto screenLightControlName = "ScreenLightControl";

    /// Control screen light and keeps it's current state
    class ScreenLightController : public ::screen_light_control::ScreenLightController
    {
      public:
        using Action                            = ::screen_light_control::Action;
        using Parameters                        = ::screen_light_control::Parameters;
        using ScreenLightMode                   = ::screen_light_control::ScreenLightMode;
        using ManualModeParameters              = ::screen_light_control::ManualModeParameters;
        using LinearProgressModeParameters      = ::screen_light_control::LinearProgressModeParameters;
        using ConstLinearProgressModeParameters = ::screen_light_control::ConstLinearProgressModeParameters;

        explicit ScreenLightController(sys::Service *parent);
        ~ScreenLightController() override;

        auto processRequest(Action action) -> void override;
        auto processRequest(Action action, const Parameters &params) -> void override;

        [[nodiscard]] auto isLightOn() const noexcept -> bool override;
        [[nodiscard]] auto isAutoModeOn() const noexcept -> bool override;
        [[nodiscard]] auto getBrightnessValue() const noexcept -> bsp::eink_frontlight::BrightnessPercentage override;
        [[nodiscard]] auto isFadeOutOngoing() -> bool override;

      private:
        auto controlTimerCallback() -> void;

        auto enableTimers() -> void;
        auto disableTimers() -> void;

        auto setParameters(const LinearProgressModeParameters &params) -> void;
        auto setParameters(const ConstLinearProgressModeParameters &params) -> void;
        auto setBrightnessInstant(bsp::eink_frontlight::BrightnessPercentage brightness) -> void;

        auto setAutomaticModeFunctions(const LinearProgressModeParameters::LinearFunctions &functions) -> void;
        ::screen_light_control::functions::LinearProgressFunction getNextAutomaticFunction();
        auto setUpAutomaticFunction(::screen_light_control::functions::LinearProgressFunction function,
                                    bsp::eink_frontlight::BrightnessPercentage currentBrightness) -> void;

        auto turnOff() -> void;
        auto turnOn(const std::optional<ManualModeParameters> &params = std::nullopt) -> void;

        auto cpuSentinelKeepOn() -> void;
        auto cpuSentinelRelease() -> void;

        bsp::eink_frontlight::BrightnessPercentage getStandarizedRampTarget(
            bsp::eink_frontlight::BrightnessPercentage target);

        static constexpr auto controlTimerIntervalMs  = 25;
        static constexpr auto rampStepPerMs           = 0.1f;

#if defined(CONFIG_VERSION_PRO) && (CONFIG_VERSION_PRO == 1)
        static constexpr auto minimalTargetBrightness = 7;
#else
        static constexpr auto minimalTargetBrightness = 15;
#endif

        sys::TimerHandle controlTimer;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        bool lightOn                                                = false;
        ScreenLightMode automaticMode                               = ScreenLightMode::Manual;
        bsp::eink_frontlight::BrightnessPercentage brightnessWhenOn = minimalTargetBrightness;
        LinearProgressModeParameters::LinearFunctions automaticModeFunctions;
    };
} // namespace bell::screen_light_control
