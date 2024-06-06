// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusTimerPresenter.hpp"
#include "FocusCommon.hpp"
#include "models/FocusSettingsModel.hpp"
#include "Application.hpp"

#include <common/models/TimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>

#include <audio/AudioMessage.hpp>

#include <LanguageUtils.hpp>

namespace
{
    constexpr auto delayBetweenSessions{std::chrono::seconds{5}};
    constexpr auto delayBetweenSessionsTimerName{"betweenSessionTimer"};
    constexpr auto summaryWindowTimeout{std::chrono::seconds{5}};

    std::string createFocusLengthText(std::time_t focusMinutes)
    {
        constexpr auto englishLanguageName = "English";

        auto focusTimeLengthString = std::to_string(focusMinutes) + " ";
        if (utils::getDisplayLanguage() == englishLanguageName) {
            focusTimeLengthString += utils::translate("common_minute_lower");
        }
        else {
            focusTimeLengthString += utils::language::getCorrectMinutesNumeralForm(focusMinutes);
        }

        return focusTimeLengthString;
    }

    std::string createSummaryText(const std::string &str, const std::string &minutesOfFocus)
    {
        auto parser = gui::text::RichTextParser{};
        const auto &result =
            parser.parse(str, nullptr, gui::text::RichTextParser::TokenMap({{"$VALUE", minutesOfFocus}}));
        return result->getText();
    }
} // namespace

namespace app::focus
{
    FocusTimerPresenter::FocusTimerPresenter(app::ApplicationCommon *app,
                                             models::FocusSettingsModel &focusTimeModel,
                                             models::FocusSettingsModel &focusRepeatsModel,
                                             models::FocusSettingsModel &shortBreakTimeModel,
                                             models::FocusSettingsModel &longBreakTimeModel,
                                             models::FocusSettingsModel &longBreakOccurrenceModel,
                                             std::unique_ptr<AbstractTimeModel> timeModel)
        : app{app}, timeModel{std::move(timeModel)}
    {
        focusSessionDuration  = std::chrono::minutes{focusTimeModel.getValue()};
        shortBreakDuration    = std::chrono::minutes{shortBreakTimeModel.getValue()};
        allFocusSessionsCount = focusRepeatsModel.getValue();
        focusSessionsLeft     = allFocusSessionsCount;
        longBreakDuration     = std::chrono::minutes{longBreakTimeModel.getValue()};
        longBreakOccurrence   = longBreakOccurrenceModel.getValue();

        betweenSessionTimer = sys::TimerFactory::createSingleShotTimer(
            app, delayBetweenSessionsTimerName, delayBetweenSessions, [this]([[maybe_unused]] sys::Timer &t) {
                executeNextStep();
            });
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

        const auto elapsed        = std::chrono::duration_cast<std::chrono::minutes>(timer->getElapsed());
        const auto focusMinutes   = ((currentTimerPhase == FocusTimerPhase::FocusTime) ? elapsed.count() : 0) +
                                  (allFocusSessionsCount - focusSessionsLeft) * focusSessionDuration.count();

        const auto &focusTimeLengthText = createFocusLengthText(focusMinutes);
        const auto &textToComplete = utils::translate("app_bell_focus_summary");
        const auto &summaryText         = createSummaryText(textToComplete, focusTimeLengthText);

        app->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("big_namaste_W_G",
                                                         focus::window::name::main,
                                                         summaryText,
                                                         gui::BellFinishedWindowData::ExitBehaviour::SwitchWindow,
                                                         summaryWindowTimeout));
    }

    void FocusTimerPresenter::executeNextStep()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTime:
            focusSessionsLeft--;
            currentTimerPhase = handleInfoAfterFocusPhase();
            startTime();
            break;

        case FocusTimerPhase::FocusTimeEnded:
            currentTimerPhase = handleCountdownAfterFocusPhase();
            startTime();
            break;

        case FocusTimerPhase::ShortBreakTime:
        case FocusTimerPhase::LongBreakTime:
            currentTimerPhase = FocusTimerPhase::BreakTimeEnded;
            getView()->showTimeForFocusInfo();
            startTime();
            break;

        case FocusTimerPhase::BreakTimeEnded:
            currentTimerPhase = FocusTimerPhase::FocusTime;
            getView()->showFocusSessionCountdown();
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

    void FocusTimerPresenter::playGong()
    {
        auto msg = std::make_shared<service::AudioStartPlaybackRequest>(app::focus::getFocusTimerAudioPath(),
                                                                        audio::PlaybackType::FocusTimer);
        app->bus.sendUnicast(std::move(msg), service::audioServiceName);
    }

    void FocusTimerPresenter::startTime()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTime:
            timer->reset(focusSessionDuration);
            timer->start();
            break;

        case FocusTimerPhase::ShortBreakTime:
            timer->reset(shortBreakDuration);
            timer->start();
            break;

        case FocusTimerPhase::LongBreakTime:
            timer->reset(longBreakDuration);
            timer->start();
            break;

        case FocusTimerPhase::FocusTimeEnded:
        case FocusTimerPhase::BreakTimeEnded:
        case FocusTimerPhase::AllFocusSessionsEnded:
            betweenSessionTimer.start();
            break;
        }
    }

    bool FocusTimerPresenter::isMiddleTimeBetweenBreakAndFocus()
    {
        switch (currentTimerPhase) {
        case FocusTimerPhase::FocusTimeEnded:
        case FocusTimerPhase::BreakTimeEnded:
            return true;
        default:
            return false;
        }
    }

    bool FocusTimerPresenter::isTimeForLongBreak()
    {
        const auto focusSessionsElapsed = allFocusSessionsCount - focusSessionsLeft;
        return (longBreakDuration.count() > 0) && ((focusSessionsElapsed % longBreakOccurrence) == 0);
    }

    FocusTimerPresenter::FocusTimerPhase FocusTimerPresenter::handleInfoAfterFocusPhase()
    {
        if (focusSessionsLeft == 0) {
            getView()->showEndOfAllSessionsInfo();
            return FocusTimerPhase::AllFocusSessionsEnded;
        }
        if (shortBreakDuration.count() > 0 || isTimeForLongBreak()) {
            getView()->showTimeForBreakInfo();
        }
        else {
            getView()->showTimeForFocusInfo();
        }
        return FocusTimerPhase::FocusTimeEnded;
    }

    FocusTimerPresenter::FocusTimerPhase FocusTimerPresenter::handleCountdownAfterFocusPhase()
    {
        if (isTimeForLongBreak()) {
            getView()->showLongBreakCountdown();
            return FocusTimerPhase::LongBreakTime;
        }
        if (shortBreakDuration.count() > 0) {
            getView()->showShortBreakCountdown();
            return FocusTimerPhase::ShortBreakTime;
        }
        getView()->showFocusSessionCountdown();
        return FocusTimerPhase::FocusTime;
    }
} // namespace app::focus
