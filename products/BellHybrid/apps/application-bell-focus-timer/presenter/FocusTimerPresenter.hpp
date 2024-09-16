// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <time/time_locale.hpp>

namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app

namespace app::focus
{
    namespace models
    {
        class FocusSettingsModel;
    } // namespace models

    class FocusTimerContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;

            virtual void showFocusSessionCountdown()                        = 0;
            virtual void showShortBreakCountdown()                          = 0;
            virtual void showLongBreakCountdown()                           = 0;
            virtual void showTimeForFocusInfo()                             = 0;
            virtual void showTimeForBreakInfo()                             = 0;
            virtual void showEndOfAllSessionsInfo()                         = 0;
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
            virtual void playGong()                                                 = 0;
        };
    };

    class FocusTimerPresenter : public FocusTimerContract::Presenter
    {
      public:
        FocusTimerPresenter(app::ApplicationCommon *app,
                            models::FocusSettingsModel &focusTimeModel,
                            models::FocusSettingsModel &focusRepeatsModel,
                            models::FocusSettingsModel &shortBreakTimeModel,
                            models::FocusSettingsModel &longBreakTimeModel,
                            models::FocusSettingsModel &longBreakOccurrenceModel,
                            std::unique_ptr<AbstractTimeModel> timeModel);

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
        void playGong() override;
        void startTime();

      private:
        enum class FocusTimerPhase
        {
            FocusTime,
            FocusTimeEnded,
            ShortBreakTime,
            LongBreakTime,
            BreakTimeEnded,
            AllFocusSessionsEnded
        };

        app::ApplicationCommon *app{nullptr};

        std::unique_ptr<app::TimerWithCallbacks> timer;
        sys::TimerHandle betweenSessionTimer;
        std::unique_ptr<AbstractTimeModel> timeModel;

        std::chrono::minutes focusSessionDuration;
        std::chrono::minutes shortBreakDuration;
        std::chrono::minutes longBreakDuration;
        std::uint32_t allFocusSessionsCount;
        std::uint32_t focusSessionsLeft;
        std::uint32_t longBreakOccurrence;
        FocusTimerPhase currentTimerPhase{FocusTimerPhase::FocusTime};

        void executeNextStep();
        bool isMiddleTimeBetweenBreakAndFocus();
        bool isTimeForLongBreak();
        FocusTimerPhase handleInfoAfterFocusPhase();
        FocusTimerPhase handleCountdownAfterFocusPhase();
    };
} // namespace app::focus
