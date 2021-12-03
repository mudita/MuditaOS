// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationProgressPresenter.hpp"

#include <common/models/TimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/SessionPausedWindow.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/Settings.hpp>

namespace
{
    constexpr std::chrono::minutes emptyValue{0};
} // namespace

namespace app::meditation
{
    MeditationProgressPresenter ::MeditationProgressPresenter(app::ApplicationCommon *app,
                                                              settings::Settings *settings,
                                                              std::unique_ptr<AbstractTimeModel> timeModel)
        : app{app}, settings{settings}, timeModel{std::move(timeModel)}
    {
        duration = std::chrono::minutes{
            utils::getNumericValue<int>(settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal))};
        interval = std::chrono::minutes{
            utils::getNumericValue<int>(settings->getValue(intervalDBRecordName, settings::SettingsScope::AppLocal))};
    }

    void MeditationProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onProgressFinished(); });
        if (interval != emptyValue) {
            timer->registerOnIntervalCallback([this]() { onIntervalReached(); });
        }
    }

    void MeditationProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    void MeditationProgressPresenter::start()
    {
        reinterpret_cast<app::Application *>(app)->suspendIdleTimer();
        timer->reset(std::chrono::seconds(duration), std::chrono::seconds(interval));
        timer->start();
    }

    void MeditationProgressPresenter::stop()
    {
        timer->stop();
    }

    void MeditationProgressPresenter::pause()
    {
        timer->stop();
        app->switchWindow(gui::window::session_paused::sessionPaused);
    }

    void MeditationProgressPresenter::resume()
    {
        timer->start();
    }

    void MeditationProgressPresenter::abandon()
    {
        timer->stop();
        app->switchWindow(gui::name::window::main_window);
    }

    void MeditationProgressPresenter::finish()
    {
        timer->stop();
        app->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("big_namaste_W_G", "", "", true, endWindowTimeout));
    }

    void MeditationProgressPresenter::onProgressFinished()
    {
        getView()->progressFinished();
    }

    void MeditationProgressPresenter::onIntervalReached()
    {
        if (interval != emptyValue) {
            getView()->intervalReached();
        }
    }

    void MeditationProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }
} // namespace app::meditation
