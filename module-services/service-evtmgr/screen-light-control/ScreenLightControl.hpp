// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ScreenLightControlParameters.hpp"
#include <memory>
#include <Timers/TimerHandle.hpp>
namespace settings
{
    class Settings;
} // namespace settings

namespace sys
{
    class Service;
} // namespace sys

/// Screen light control algorithm. Automatic/Manual mode of operation.
/// Processing of ambient light sensor input to screen brightness output.
namespace screen_light_control
{
    class ScreenLightController
    {
      public:
        virtual ~ScreenLightController() noexcept = default;

        virtual void processRequest(Action action)                           = 0;
        virtual void processRequest(Action action, const Parameters &params) = 0;
        [[nodiscard]] virtual auto isLightOn() const noexcept -> bool        = 0;
        [[nodiscard]] virtual auto isAutoModeOn() const noexcept -> bool     = 0;
        [[nodiscard]] virtual auto getBrightnessValue() const noexcept
            -> bsp::eink_frontlight::BrightnessPercentage = 0;
    };
} // namespace screen_light_control
