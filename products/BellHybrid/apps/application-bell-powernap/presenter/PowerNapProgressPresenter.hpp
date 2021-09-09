// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <Timers/TimerHandle.hpp>
#include <memory>
namespace app
{
    class ApplicationCommon;
} // namespace app
namespace gui
{
    class Item;
} // namespace gui
namespace settings
{
    class Settings;
}

namespace app::powernap
{
    class PowerNapAlarm;
    class PowerNapProgressContract
    {
      public:
        class View
        {
          public:
            ~View() = default;
            virtual void napEnded() = 0;
        };

        class Presenter : public BasePresenter<PowerNapProgressContract::View>
        {
          public:
            virtual void initTimer(gui::Item *parent)                              = 0;
            virtual app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept = 0;
            virtual void activate()                                                = 0;
            virtual void endNap()                                                  = 0;
        };
    };

    class AlarmController;

    class PowerNapProgressPresenter : public PowerNapProgressContract::Presenter
    {
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
        PowerNapAlarm &alarm;
        std::unique_ptr<app::ProgressTimer> timer;
        sys::TimerHandle napAlarmTimer;

        void initTimer(gui::Item *parent) override;
        app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept override;
        void activate() override;
        void endNap() override;

        void onNapFinished();
        void onNapAlarmFinished();

      public:
        PowerNapProgressPresenter(app::ApplicationCommon *app, settings::Settings *settings, PowerNapAlarm &alarm);
    };
} // namespace app::powernap
