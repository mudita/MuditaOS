// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <service-db/Settings.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::powernap
{
    class PowerNapFrontlightModel
    {
      public:
        explicit PowerNapFrontlightModel(ApplicationCommon *app, const std::chrono::seconds &alarmDuration);

        auto isAlarmLightEnabled() const -> bool;
        auto lockKeypressTrigger() -> void;
        auto unlockKeypressTrigger() -> void;
        auto startBrightnessFadeIn() -> void;
        auto turnOff() -> void;

      private:
        ApplicationCommon *app = nullptr;
        const std::chrono::seconds alarmDuration;
        mutable settings::Settings settings;

        auto getAlarmBrightness() const -> int;
        auto prepareFadeInParameters() const -> screen_light_control::LinearProgressModeParameters;
    };
} // namespace app::powernap
