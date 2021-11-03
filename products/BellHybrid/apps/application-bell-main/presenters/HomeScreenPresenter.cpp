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
        handleCyclicDeepRerfesh(CyclicDeepRefreshIntervalMinutes);
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

    void HomeScreenPresenter::setSnoozeTimer(std::unique_ptr<app::ProgressTimerWithSnoozeTimer> &&_timer)
    {
        snoozeTimer = std::move(_timer);
    }

    void HomeScreenPresenter::startSnoozeTimer(std::chrono::seconds snoozeDuration)
    {
        snoozeTimer->reset(snoozeDuration, snoozeTick);
        snoozeTimer->start();
    }

    void HomeScreenPresenter::stopSnoozeTimer()
    {
        snoozeTimer->start();
    }

    void HomeScreenPresenter::restartSnoozeTimer(std::chrono::seconds snoozeDuration)
    {
        snoozeTimer->reset(snoozeDuration, snoozeTick);
    }

    std::uint32_t HomeScreenPresenter::getBatteryLvl() const
    {
        return batteryModel->getLevelState().level;
    }

    bool HomeScreenPresenter::isBatteryCharging() const
    {
        return batteryModel->getLevelState().state == Store::Battery::State::Charging;
    }

    void HomeScreenPresenter::handleCyclicDeepRerfesh(const int interval) 
    {
        auto currTime = timeModel->getCurrentTime();
        auto currStructTime = std::localtime(&currTime);

        if (((currStructTime->tm_min % interval) == 0) && (currStructTime->tm_sec == 0))   //true only when interval criteria on minutes (eg. every 30 min) met
        {
            app->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
            vTaskDelay(pdMS_TO_TICKS(CyclicDeepRefreshHaltAfterRefreshMsec));
        }
    }
} // namespace app::home_screen
