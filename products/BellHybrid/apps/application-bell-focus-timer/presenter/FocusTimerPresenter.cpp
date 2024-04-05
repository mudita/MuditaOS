// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusTimerPresenter.hpp"

#include "FocusCommon.hpp"
#include "Application.hpp"
#include "widgets/ProgressTimer.hpp"

#include <common/LanguageUtils.hpp>
#include <common/models/TimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <service-db/Settings.hpp>

namespace
{
    static constexpr std::chrono::milliseconds betweenSessionDelayTime{5000};
} // namespace

namespace app::focus
{

    FocusTimerPresenter::FocusTimerPresenter(app::ApplicationCommon *app,
                                             settings::Settings *settings,
                                             std::unique_ptr<AbstractTimeModel> timeModel,
                                             AbstractBatteryModel &batteryModel,
                                             AbstractLowBatteryInfoModel &lowBatteryInfoModel)
        : app{app}, settings{settings}, batteryModel{batteryModel},
          lowBatteryInfoModel{lowBatteryInfoModel}, timeModel{std::move(timeModel)}
    {
        //        focusSessionDuration = std::chrono::minutes{
        //                utils::getNumericValue<int>(settings->getValue(focusDBRecordName,
        //                settings::SettingsScope::AppLocal))};
        //        shortBreakDuration = std::chrono::minutes{
        //                utils::getNumericValue<int>(settings->getValue(focusDBRecordName,
        //                settings::SettingsScope::AppLocal))};
        //        focusSessionsLeft =
        //                utils::getNumericValue<int>(settings->getValue(focusDBRecordName,
        //                settings::SettingsScope::AppLocal));

        focusSessionDuration = std::chrono::minutes{25};
        shortBreakDuration   = std::chrono::minutes{5};

        allFocusSessionsCount = 10;
        focusSessionsLeft     = allFocusSessionsCount;

        betweenSessionTimer = sys::TimerFactory::createSingleShotTimer(
            app, "betweenSessionTimer", betweenSessionDelayTime, [this](sys::Timer &) { executeNextStep(); });
    }

    void FocusTimerPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { executeNextStep(); });
    }

    void FocusTimerPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    void FocusTimerPresenter::start()
    {
        static_cast<app::Application *>(app)->suspendIdleTimer();
        startTime();
    }

    void FocusTimerPresenter::stop()
    {
        finish();
    }

    bool FocusTimerPresenter::isTimerStopped()
    {
        return timer->isStopped();
    }

    bool FocusTimerPresenter::isAllSessionsFinished()
    {
        return currentTimerPhase == FocusTimerPhase::AllFocusSessionsEnded;
    }

    void FocusTimerPresenter::pause()
    {
        betweenSessionTimer.stop();
        timer->stop();
        getView()->pause();
    }

    void FocusTimerPresenter::resume()
    {
        isMiddleTimeBetweenBreakAndFocus() ? executeNextStep() : timer->start();
        getView()->resume();
    }

    void FocusTimerPresenter::abandon()
    {
        finish();
    }

    void FocusTimerPresenter::finish()
    {
        timer->stop();
        betweenSessionTimer.stop();
        app->switchWindow(focus::window::name::main);
    }

    void FocusTimerPresenter::executeNextStep()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTime:
            focusSessionsLeft--;
            if (focusSessionsLeft == 0) {
                currentTimerPhase = FocusTimerPhase::AllFocusSessionsEnded;
                getView()->onAllFocusSessionsFinished();
                startTime();
            }
            else {
                currentTimerPhase = FocusTimerPhase::FocusTimeEnded;
                getView()->onFocusSessionFinished();
                startTime();
            }
            break;

        case FocusTimerPhase::FocusTimeEnded:
            currentTimerPhase = FocusTimerPhase::ShortBreakTime;
            getView()->onShortBreakStarted();
            startTime();
            break;

        case FocusTimerPhase::ShortBreakTime:
            currentTimerPhase = FocusTimerPhase::ShortBreakTimeEnded;
            getView()->onShortBreakFinished();
            startTime();
            break;

        case FocusTimerPhase::ShortBreakTimeEnded:
            currentTimerPhase = FocusTimerPhase::FocusTime;
            getView()->onFocusSessionStarted();
            startTime();
            break;

        case FocusTimerPhase::AllFocusSessionsEnded:
            finish();
            break;
        }
    }

    void FocusTimerPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    void FocusTimerPresenter::startTime()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTime:
            timer->reset(std::chrono::minutes(focusSessionDuration));
            timer->start();
            break;

        case FocusTimerPhase::ShortBreakTime:
            timer->reset(std::chrono::minutes(shortBreakDuration));
            timer->start();
            break;

        case FocusTimerPhase::FocusTimeEnded:
        case FocusTimerPhase::ShortBreakTimeEnded:
        case FocusTimerPhase::AllFocusSessionsEnded:
            betweenSessionTimer.start();
            break;
        }
    }

    bool FocusTimerPresenter::isMiddleTimeBetweenBreakAndFocus()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTimeEnded:
        case FocusTimerPhase::ShortBreakTimeEnded:
            return true;
        default:
            return false;
        }
    }

} // namespace app::focus
