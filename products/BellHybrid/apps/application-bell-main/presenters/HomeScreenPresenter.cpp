// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-main/presenters/HomeScreenPresenter.hpp"
#include "StateController.hpp"
#include "models/TemperatureModel.hpp"
#include "models/UsbStatusModel.hpp"
#include "models/DemoModel.hpp"

#include <windows/BellHomeScreenWindow.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/widgets/ProgressTimerWithSnoozeTimer.hpp>
#include <common/windows/BellBatteryStatusWindow.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>
#include <Timers/SystemTimer.hpp>
#include <Timers/TimerFactory.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QuotesMessages.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <switches/LatchStatusRequest.hpp>

#include <bsp/trng/trng.hpp>

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
} // namespace

namespace app::home_screen
{
    HomeScreenPresenter::HomeScreenPresenter(ApplicationCommon *app,
                                             AbstractAlarmModel &alarmModel,
                                             AbstractBatteryModel &batteryModel,
                                             AbstractTemperatureModel &temperatureModel,
                                             AbstractTimeModel &timeModel,
                                             AbstractUserSessionModel &userSessionModel,
                                             AbstractBatteryLevelNotificationModel &batteryLevelNotificationModel,
                                             AbstractUsbStatusModel &usbStatusModel,
                                             AbstractQuoteModel &quoteModel,
                                             AbstractDemoModel &demoModel)
        : app{app}, alarmModel{alarmModel}, batteryModel{batteryModel},
          temperatureModel{temperatureModel}, timeModel{timeModel}, userSessionModel{userSessionModel},
          batteryLevelNotificationModel{batteryLevelNotificationModel}, usbStatusModel{usbStatusModel},
          quoteModel{quoteModel}, demoModel{demoModel}, rngEngine{
                                                            std::make_unique<std::mt19937>(bsp::trng::getRandomValue())}
    {}

    gui::RefreshModes HomeScreenPresenter::handleUpdateTimeEvent()
    {
        requestQuote();
        getView()->setTime(timeModel.getCurrentTime());
        stateController->handleTimeUpdateEvent();
        return handleCyclicDeepRefresh();
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
        requestQuote();
        stateController->resetStateMachine();
        getView()->setTimeFormat(timeModel.getTimeFormat());
        getView()->setTime(timeModel.getCurrentTime());
        getView()->setAlarmTimeFormat(timeModel.getTimeFormat());
        getView()->setSnoozeFormat(timeModel.getTimeFormat());
        getView()->setTemperature(temperatureModel.getTemperature());
        updateUsbStatus();
    }
    void HomeScreenPresenter::createData()
    {
        stateController = std::make_unique<StateController>(
            *getView(), *this, batteryModel, temperatureModel, alarmModel, timeModel, userSessionModel, demoModel);
    }

    void HomeScreenPresenter::refreshWindow()
    {
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }
    void HomeScreenPresenter::onDatabaseMessage(db::NotificationMessage *msg)
    {
        if (msg->interface == db::Interface::Name::AlarmEvents && msg->type == db::Query::Type::Update) {
            alarmModel.update();
        }
    }
    void HomeScreenPresenter::handleAlarmModelReady()
    {
        getView()->setAlarmTime(alarmModel.getAlarmTime());
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
        return batteryModel.getLevelState().level;
    }

    bool HomeScreenPresenter::isBatteryCharging() const
    {
        return batteryModel.getLevelState().state == Store::Battery::State::Charging;
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

    gui::RefreshModes HomeScreenPresenter::handleCyclicDeepRefresh()
    {
        constexpr auto deepRefreshPeriod = 30;
        static auto refreshCount         = 0;
        gui::RefreshModes refresh{gui::RefreshModes::GUI_REFRESH_FAST};

        if (refreshCount >= deepRefreshPeriod) {
            refresh      = gui::RefreshModes::GUI_REFRESH_DEEP;
            refreshCount = 0;
        }
        refreshCount++;
        return refresh;
    }
    void HomeScreenPresenter::handleBatteryStatus()
    {
        stateController->handleBatteryStatus();
        updateUsbStatus();
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
        app->switchWindow(gui::BellBatteryStatusWindow::windowName,
                          std::make_unique<gui::BatteryStatusSwitchData>(getBatteryLvl(), isBatteryCharging()));
    }

    void HomeScreenPresenter::switchToLowBatteryWarning()
    {
        app->switchWindow(gui::BellBatteryStatusWindow::windowName,
                          std::make_unique<gui::BatteryStatusSwitchData>(getBatteryLvl(), isBatteryCharging(), true));
    }

    UTF8 HomeScreenPresenter::getGreeting()
    {
        const auto greetingCollection = utils::translate_array("app_bell_greeting_msg");
        if (greetingCollection.empty()) {
            LOG_WARN("Array 'app_bell_greeting_msg' does not exist, using default string");
            return "app_bell_greeting_msg";
        }

        std::uniform_int_distribution<std::mt19937::result_type> dist(0, greetingCollection.size() - 1);
        return greetingCollection[dist(*rngEngine)];
    };

    void HomeScreenPresenter::updateUsbStatus()
    {
        const auto batteryState   = batteryModel.getLevelState().state;
        const bool isUsbConnected = usbStatusModel.isUsbConnected(batteryState);
        getView()->updateUsbStatus(isUsbConnected);
    }

    bool HomeScreenPresenter::isLowBatteryWarningNeeded()
    {
        batteryLevelNotificationModel.updateBatteryLevelInterval(getBatteryLvl());
        return !userSessionModel.isActiveUserSessionHandled() &&
               !batteryLevelNotificationModel.isBatteryIntervalHandled() && !isBatteryCharging();
    }

    void HomeScreenPresenter::handleLowBatteryWarning()
    {
        if (batteryLevelNotificationModel.getBatteryLevelInterval() != BatteryLevelInterval::Unknown &&
            batteryLevelNotificationModel.getBatteryLevelInterval() != BatteryLevelInterval::Above15Percent) {
            switchToLowBatteryWarning();
        }
        else {
            userSessionModel.setCurrentUserSessionAsHandled();
        }
        batteryLevelNotificationModel.setCurrentBatteryIntervalAsHandled();
        userSessionModel.setCurrentUserSessionAsHandled();
    }

    void HomeScreenPresenter::refreshUserSession()
    {
        userSessionModel.activateUserSession();
    }

    void HomeScreenPresenter::updateBatteryLevelInterval()
    {
        batteryLevelNotificationModel.updateBatteryLevelInterval(getBatteryLvl());
    }

    bool HomeScreenPresenter::isLowBatteryLevel() const
    {
        const auto batteryStatus = batteryModel.getLevelState();
        return not batteryModel.isBatteryCharging(batteryStatus.state) &&
               (batteryStatus.level < constants::lowBatteryInfoThreshold);
    }

    void HomeScreenPresenter::requestQuote() const
    {
        quoteModel.setCallback(
            [this](std::string quote, std::string author) { getView()->setQuoteText(quote, author); });
        quoteModel.sendQuery();
    }

    void HomeScreenPresenter::handleDemoMode()
    {
        stateController->handleDemoMode();
    }

    void HomeScreenPresenter::endDemoMode()
    {
        stateController->handleEndDemoMode();
    }

    void HomeScreenPresenter::showVolumeScreen()
    {
        app->switchWindow(gui::popup::window::volume_window);
    }

} // namespace app::home_screen
