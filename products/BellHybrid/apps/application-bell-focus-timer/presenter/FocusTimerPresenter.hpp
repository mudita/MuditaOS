// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <time/time_locale.hpp>
#include "widgets/ProgressTimer.hpp"

namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app

namespace app::focus
{
    class FocusTimerContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept                                        = default;
            virtual void onAllFocusSessionsFinished()                       = 0;
            virtual void onFocusSessionStarted()                            = 0;
            virtual void onFocusSessionFinished()                           = 0;
            virtual void onShortBreakStarted()                              = 0;
            virtual void onShortBreakFinished()                             = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
            virtual void pause()                                            = 0;
            virtual void resume()                                           = 0;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept                                           = default;
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) = 0;
            virtual void handleUpdateTimeEvent()                                    = 0;
            virtual bool isTimerStopped()                                           = 0;
            virtual bool isAllSessionsFinished()                                    = 0;
            virtual void start()                                                    = 0;
            virtual void stop()                                                     = 0;
            virtual void pause()                                                    = 0;
            virtual void resume()                                                   = 0;
            virtual void abandon()                                                  = 0;
            virtual void finish()                                                   = 0;
            virtual void onBeforeShow()                                             = 0;
        };
    };

    class FocusTimerPresenter : public FocusTimerContract::Presenter
    {
      public:
        enum class FocusTimerPhase
        {
            FocusTime,
            FocusTimeEnded,
            ShortBreakTime,
            ShortBreakTimeEnded,
            AllFocusSessionsEnded
        };
        FocusTimerPresenter(app::ApplicationCommon *app,
                            settings::Settings *settings,
                            std::unique_ptr<AbstractTimeModel> timeModel,
                            AbstractBatteryModel &batteryModel,
                            AbstractLowBatteryInfoModel &lowBatteryInfoModel);

        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void handleUpdateTimeEvent() override;
        bool isTimerStopped() override;
        bool isAllSessionsFinished() override;
        void start() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void abandon() override;
        void finish() override;
        void onBeforeShow() override;
        void startTime();

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;

        std::unique_ptr<app::TimerWithCallbacks> timer;
        sys::TimerHandle betweenSessionTimer;
        std::unique_ptr<AbstractTimeModel> timeModel;
        std::chrono::minutes focusSessionDuration;
        std::chrono::minutes shortBreakDuration;
        std::uint32_t allFocusSessionsCount;
        std::uint32_t focusSessionsLeft;
        FocusTimerPhase currentTimerPhase = FocusTimerPhase::FocusTime;

        void executeNextStep();
        bool isMiddleTimeBetweenBreakAndFocus();
    };
} // namespace app::focus
