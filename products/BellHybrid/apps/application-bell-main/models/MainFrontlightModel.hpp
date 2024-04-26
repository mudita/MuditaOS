// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <service-db/Settings.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    class AbstractFrontlightModel
    {
      public:
        virtual ~AbstractFrontlightModel() noexcept = default;

        virtual void StartBrightnessFadeIn(std::chrono::seconds duration) = 0;
        virtual void TurnOff()                                            = 0;
        virtual void LockKeypressTrigger()                                = 0;
        virtual void UnlockKeypressTrigger()                              = 0;
    };

    class MainFrontlightModel : public AbstractFrontlightModel
    {
      public:
        explicit MainFrontlightModel(ApplicationCommon *app);

        void StartBrightnessFadeIn(std::chrono::seconds duration) override;
        void TurnOff() override;
        void LockKeypressTrigger() override;
        void UnlockKeypressTrigger() override;

      private:
        using FadeInParams = screen_light_control::LinearProgressModeParameters;

        ApplicationCommon *app{nullptr};
        mutable settings::Settings settings;

        int GetAlarmBrightness() const;
        FadeInParams PrepareFadeInParameters(std::chrono::seconds duration) const;
    };
} // namespace app
