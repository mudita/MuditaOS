// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-main/presenters/HomeScreenPresenter.hpp"
#include "StateController.hpp"
#include "models/BatteryModel.hpp"
#include "models/TemperatureModel.hpp"

#include <windows/BellHomeScreenWindow.hpp>
#include <windows/BellBatteryStatusWindow.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <common/models/TimeModel.hpp>
#include <common/widgets/ProgressTimerWithSnoozeTimer.hpp>
#include <Timers/SystemTimer.hpp>
#include <Timers/TimerFactory.hpp>
#include <time/time_constants.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include <switches/LatchStatusRequest.hpp>

namespace
{
    using utils::time::Locale;
    void increaseHour(struct tm &tm)
    {
        if (tm.tm_hour >= Locale::max_hour_24H_mode) {
            tm.tm_hour = 0;
            tm.tm_min  = 0;
        }
        else {
            tm.tm_hour++;
            tm.tm_min = 0;
        }
    }

    void decreaseHour(struct tm &tm)
    {
        if (tm.tm_hour <= Locale::min_hour_24H_mode) {
            tm.tm_hour = Locale::max_hour_24H_mode;
            tm.tm_min  = Locale::max_minutes;
        }
        else {
            tm.tm_hour--;
            tm.tm_min = utils::time::Locale::max_minutes;
        }
    }

    void handleMinuteIncrease(struct tm &tm)
    {
        if (tm.tm_min >= Locale::max_minutes) {
            increaseHour(tm);
        }
        else {
            tm.tm_min++;
        }
    }

    void handleMinuteDecrease(struct tm &tm)
    {
        if (tm.tm_min <= 0) {
            decreaseHour(tm);
        }
        else {
            tm.tm_min--;
        }
    }
}; // namespace
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
        handleCyclicDeepRefresh();
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
        stateController->resetStateMachine();
        getView()->setTimeFormat(timeModel->getTimeFormat());
        getView()->setTime(timeModel->getCurrentTime());
        getView()->setAlarmTimeFormat(timeModel->getTimeFormat());
        getView()->setSnoozeFormat(timeModel->getTimeFormat());
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
        snoozeTimer->stop();
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

    bool HomeScreenPresenter::isAlarmActivatedByLatch() const
    {
        constexpr auto timeout = pdMS_TO_TICKS(1500);

        auto response =
            app->bus.sendUnicastSync(std::make_shared<sevm::LatchStatusRequest>(), service::name::evt_manager, timeout);

        if (response.first == sys::ReturnCodes::Success) {
            auto msgState = dynamic_cast<sevm::LatchStatusResponse *>(response.second.get());
            if (msgState != nullptr) {
                return msgState->getStatus() != sevm::LatchStatus::PRESSED;
            }
        }

        LOG_FATAL("Couldn't fetch latch state");
        return false;
    }

    void HomeScreenPresenter::handleCyclicDeepRefresh()
    {
        constexpr auto deepRefreshPeriod = 30;
        static auto refreshCount         = 0;

        if (refreshCount >= deepRefreshPeriod) {
            app->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
            refreshCount = 0;
        }
        refreshCount++;
    }
    void HomeScreenPresenter::handleBatteryStatus()
    {
        stateController->handleBatteryStatus();
    }

    void HomeScreenPresenter::setLayout(gui::LayoutGenerator layoutGenerator)
    {
        getView()->setLayout(layoutGenerator);
    }

    void HomeScreenPresenter::incAlarmMinute()
    {
        const auto alarmTime = getView()->getAlarmTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteIncrease(*newTime);
        getView()->setAlarmTime(std::mktime(newTime));
    }

    void HomeScreenPresenter::decAlarmMinute()
    {
        const auto alarmTime = getView()->getAlarmTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteDecrease(*newTime);
        getView()->setAlarmTime(std::mktime(newTime));
    }

    void HomeScreenPresenter::switchToMenu()
    {
        app->switchWindow(gui::window::name::bell_main_menu, nullptr);
    }

    void HomeScreenPresenter::switchToBatteryStatus()
    {
        app->switchWindow(gui::BellBatteryStatusWindow::name,
                          std::make_unique<gui::BellBatteryStatusWindow::Data>(getBatteryLvl(), isBatteryCharging()));
    }
} // namespace app::home_screen
