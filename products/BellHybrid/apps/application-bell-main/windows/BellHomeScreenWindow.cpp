// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellHomeScreenWindow.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/SnoozeTimer.hpp"
#include "BellBatteryStatusWindow.hpp"
#include "ProductConfig.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <i18n/i18n.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

#include <chrono>

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
    inline constexpr auto snoozeTimerName = "SnoozeTimer";
    inline constexpr std::chrono::seconds timerTick{1};
} // namespace

namespace gui
{
    BellHomeScreenWindow::BellHomeScreenWindow(app::ApplicationCommon *app,
                                               std::unique_ptr<app::home_screen::AbstractPresenter> presenter,
                                               std::vector<std::shared_ptr<BaseHomeScreenLayout>> layouts)
        : AppWindow(app, name::window::main_window), presenter{std::move(presenter)}, layouts{std::move(layouts)}
    {
        buildInterface();
        setLayout("Classic");
        this->presenter->attach(this);
        this->presenter->createData();
        this->presenter->onBeforeShow();
    }

    void BellHomeScreenWindow::buildInterface()
    {
        using namespace bellMainStyle;
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);
    }

    void BellHomeScreenWindow::setLayout(std::string layoutName)
    {
        std::shared_ptr<BaseHomeScreenLayout> layoutFound;
        for (auto layout : layouts) {
            if (layout->getName() == layoutName) {
                layoutFound = layout;
            }
        }
        if (currentLayout) {
            removeWidget(currentLayout.get());
        }
        currentLayout = layoutFound;
        addWidget(currentLayout.get());

        if (auto snoozeTimer = currentLayout->getSnoozeTimer()) {
            auto timer = std::make_unique<app::ProgressTimerWithSnoozeTimer>(
                application, *this, snoozeTimerName, timerTick, app::ProgressCountdownMode::Increasing);
            timer->attach(snoozeTimer);
            presenter->setSnoozeTimer(std::move(timer));
        }
    }

    void BellHomeScreenWindow::setAlarmTriggered()
    {
        currentLayout->setAlarmTriggered();
    }

    void BellHomeScreenWindow::setAlarmActive(bool val)
    {
        currentLayout->setAlarmActive(val);
    }

    void BellHomeScreenWindow::setViewState(app::home_screen::ViewState state)
    {
        currentLayout->setViewState(state);
    }

    void BellHomeScreenWindow::setTemperature(utils::temperature::Temperature newTemp)
    {
        currentLayout->setTemperature(newTemp);
    }

    void BellHomeScreenWindow::setBottomDescription(const UTF8 &desc)
    {
        currentLayout->setBottomDescription(desc);
    }

    void BellHomeScreenWindow::removeBottomDescription()
    {
        currentLayout->removeBottomDescription();
    }

    void BellHomeScreenWindow::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        currentLayout->setBatteryLevelState(batteryContext);
    }

    void BellHomeScreenWindow::setTime(std::time_t newTime)
    {
        currentLayout->setTime(newTime);
    }

    void BellHomeScreenWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        currentLayout->setTimeFormat(fmt);
    }

    void BellHomeScreenWindow::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        currentLayout->setAlarmTimeFormat(fmt);
    }

    void BellHomeScreenWindow::setSnoozeFormat(utils::time::Locale::TimeFormat fmt)
    {
        currentLayout->setSnoozeFormat(fmt);
    }

    void BellHomeScreenWindow::setAlarmEdit(bool val)
    {
        currentLayout->setAlarmEdit(val);
    }

    std::time_t BellHomeScreenWindow::getAlarmTime() const
    {
        return currentLayout->getAlarmTime();
    }

    void BellHomeScreenWindow::setAlarmTime(std::time_t newTime)
    {
        currentLayout->setAlarmTime(newTime);
    }

    void BellHomeScreenWindow::incAlarmMinute()
    {
        const auto alarmTime = currentLayout->getAlarmTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteIncrease(*newTime);
        currentLayout->setAlarmTime(std::mktime(newTime));
    }

    void BellHomeScreenWindow::decAlarmMinute()
    {
        const auto alarmTime = currentLayout->getAlarmTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteDecrease(*newTime);
        currentLayout->setAlarmTime(std::mktime(newTime));
    }

    void BellHomeScreenWindow::setSnoozeTime(std::time_t newTime)
    {
        currentLayout->setSnoozeTime(newTime);
    }
    /////////// NON-LAYOUT METHODS
    void BellHomeScreenWindow::switchToMenu()
    {
        application->switchWindow(window::name::bell_main_menu, nullptr);
    }

    bool BellHomeScreenWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease() || inputEvent.isLongRelease()) {
            return presenter->handleInputEvent(inputEvent);
        }
        return false;
    }

    void BellHomeScreenWindow::onBeforeShow(ShowMode, SwitchData *data)
    {
        const auto alarmRingingSwitchData = dynamic_cast<app::actions::AlarmRingingData *>(data);
        if (alarmRingingSwitchData != nullptr) {
            presenter->handleAlarmRingingEvent();
        }
        else {
            presenter->onBeforeShow();
        }
    }
    bool BellHomeScreenWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return true;
    }
    void BellHomeScreenWindow::switchToBatteryStatus()
    {
        // These changes are only to demostrate layout changes easily via long back press
        // depo purposes only
        static int i = 1;
        LOG_DEBUG(">>>>>>>>>>>>>>> Going to: %d:  %s", i, layouts[(i) % layouts.size()]->getName().c_str());
        setLayout(layouts[(i++) % layouts.size()]->getName());
        presenter->onBeforeShow();

        // application->switchWindow(gui::BellBatteryStatusWindow::name,
        //                          std::make_unique<gui::BellBatteryStatusWindow::Data>(presenter->getBatteryLvl(),
        //                                                                               presenter->isBatteryCharging()));
    }

    bool BellHomeScreenWindow::updateBatteryStatus()
    {
        presenter->handleBatteryStatus();
        return true;
    }

    bool BellHomeScreenWindow::onDatabaseMessage(sys::Message *msg)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msg);
        if (msgNotification != nullptr) {
            presenter->onDatabaseMessage(msgNotification);
            return true;
        }
        return false;
    }

} // namespace gui
