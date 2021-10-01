// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HomeScreenPresenter.hpp"
#include "StateController.hpp"
#include "models/BatteryModel.hpp"
#include "models/TemperatureModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <common/models/TimeModel.hpp>
#include <Timers/SystemTimer.hpp>
#include <Timers/TimerFactory.hpp>
#include <time/time_constants.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace app::home_screen
{
    HomeScreenPresenter::HomeScreenPresenter(ApplicationCommon *app,
                                             std::unique_ptr<AbstractAlarmModel> alarmModel,
                                             std::unique_ptr<AbstractBatteryModel> batteryModel,
                                             std::unique_ptr<AbstractTemperatureModel> temperatureModel,
                                             std::unique_ptr<AbstractTimeModel> timeModel)
        : app{app}, alarmModel{std::move(alarmModel)}, batteryModel{std::move(batteryModel)},
          temperatureModel{std::move(temperatureModel)}, timeModel{std::move(timeModel)}
    {}

    void HomeScreenPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
        stateController->handleTimeUpdateEvent();
    }

    void HomeScreenPresenter::handleAlarmRingingEvent()
    {
        stateController->handleAlarmRingingEvent();
    }

    bool HomeScreenPresenter::handleInputEvent(const gui::InputEvent &inputEvent)
    {
        return stateController->handleInputEvent(inputEvent);
    }

    void HomeScreenPresenter::spawnTimer(std::chrono::milliseconds timeout)
    {
        if (not timer.isValid()) {
            auto callback = [this](sys::Timer &) { stateController->handleTimerEvent(); };
            timer         = sys::TimerFactory::createSingleShotTimer(app, timerName, timeout, callback);
        }
        timer.stop();
        timer.start();
    }

    void HomeScreenPresenter::detachTimer()
    {
        if (timer.isValid()) {
            timer.stop();
            timer.reset();
        }
    }

    HomeScreenPresenter::~HomeScreenPresenter()
    {
        detachTimer();
    }

    void HomeScreenPresenter::onBeforeShow()
    {
        alarmModel->update([&]() { handleAlarmModelReady(); });
        getView()->setTimeFormat(timeModel->getTimeFormat());
        getView()->setTime(timeModel->getCurrentTime());
        getView()->setAlarmTimeFormat(timeModel->getTimeFormat());
        getView()->setTemperature(temperatureModel->getTemperature());
    }
    void HomeScreenPresenter::createData()
    {
        stateController = std::make_unique<StateController>(
            *getView(), *this, *batteryModel, *temperatureModel, *alarmModel, *timeModel);
    }

    void HomeScreenPresenter::refreshWindow()
    {
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }
    void HomeScreenPresenter::onDatabaseMessage(db::NotificationMessage *msg)
    {
        if (msg->interface == db::Interface::Name::AlarmEvents && msg->type == db::Query::Type::Update) {
            alarmModel->update();
        }
    }
    void HomeScreenPresenter::handleAlarmModelReady()
    {
        getView()->setAlarmTime(alarmModel->getAlarmTime());
        stateController->handleAlarmModelReady();
    }
} // namespace app::home_screen
