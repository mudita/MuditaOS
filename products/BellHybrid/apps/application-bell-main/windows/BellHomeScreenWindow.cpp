// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellHomeScreenWindow.hpp"
#include "BellBatteryStatusWindow.hpp"
#include "ProductConfig.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/actions/AlarmRingingData.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/widgets/SnoozeTimer.hpp>
#include <common/widgets/ProgressTimerWithSnoozeTimer.hpp>
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
                                               std::shared_ptr<app::home_screen::AbstractPresenter> presenter)
        : AppWindow(app, name::window::main_window), presenter{presenter}
    {
        buildInterface();
        this->presenter->attach(this);
        this->presenter->createData();
    }

    void BellHomeScreenWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);
    }

    void BellHomeScreenWindow::setLayout(LayoutGenerator layoutGenerator)
    {
        if (currentLayout) {
            removeWidget(currentLayout->getLayout());
        }
        currentLayout = layoutGenerator();
        addWidget(static_cast<BellBaseLayout *>(currentLayout->getLayout()));
        presenter->onBeforeShow();

        if (auto snoozeTimer = currentLayout->getSnoozeTimer()) {
            auto timer = std::make_unique<app::ProgressTimerWithSnoozeTimer>(
                application, *this, snoozeTimerName, timerTick, app::ProgressCountdownMode::Increasing);
            timer->attach(snoozeTimer);
            presenter->setSnoozeTimer(std::move(timer));
        }
    }

    void BellHomeScreenWindow::setAlarmTriggered()
    {
        if (currentLayout) {
            currentLayout->setAlarmTriggered();
        }
    }

    void BellHomeScreenWindow::setAlarmActive(bool val)
    {
        if (currentLayout) {
            currentLayout->setAlarmActive(val);
        }
    }

    void BellHomeScreenWindow::setViewState(app::home_screen::ViewState state)
    {
        if (currentLayout) {
            currentLayout->setViewState(state);
        }
    }

    void BellHomeScreenWindow::setTemperature(utils::temperature::Temperature newTemp)
    {
        if (currentLayout) {
            currentLayout->setTemperature(newTemp);
        }
    }

    void BellHomeScreenWindow::setBottomDescription(const UTF8 &desc)
    {
        if (currentLayout) {
            currentLayout->setBottomDescription(desc);
        }
    }

    void BellHomeScreenWindow::removeBottomDescription()
    {
        if (currentLayout) {
            currentLayout->removeBottomDescription();
        }
    }

    void BellHomeScreenWindow::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        if (currentLayout) {
            currentLayout->setBatteryLevelState(batteryContext);
        }
    }

    void BellHomeScreenWindow::setTime(std::time_t newTime)
    {
        if (currentLayout) {
            currentLayout->setTime(newTime);
        }
    }

    void BellHomeScreenWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        if (currentLayout) {
            currentLayout->setTimeFormat(fmt);
        }
    }

    void BellHomeScreenWindow::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        if (currentLayout) {
            currentLayout->setAlarmTimeFormat(fmt);
        }
    }

    void BellHomeScreenWindow::setSnoozeFormat(utils::time::Locale::TimeFormat fmt)
    {
        if (currentLayout) {
            currentLayout->setSnoozeFormat(fmt);
        }
    }

    void BellHomeScreenWindow::setAlarmEdit(bool val)
    {
        if (currentLayout) {
            currentLayout->setAlarmEdit(val);
        }
    }

    std::time_t BellHomeScreenWindow::getAlarmTime() const
    {
        if (currentLayout) {
            return currentLayout->getAlarmTime();
        }
        else {
            return {};
        }
    }

    void BellHomeScreenWindow::setAlarmTime(std::time_t newTime)
    {
        if (currentLayout) {
            currentLayout->setAlarmTime(newTime);
        }
    }

    void BellHomeScreenWindow::incAlarmMinute()
    {
        if (currentLayout) {
            const auto alarmTime = currentLayout->getAlarmTime();
            auto newTime         = std::localtime(&alarmTime);
            handleMinuteIncrease(*newTime);
            currentLayout->setAlarmTime(std::mktime(newTime));
        }
    }

    void BellHomeScreenWindow::decAlarmMinute()
    {
        if (currentLayout) {
            const auto alarmTime = currentLayout->getAlarmTime();
            auto newTime         = std::localtime(&alarmTime);
            handleMinuteDecrease(*newTime);
            currentLayout->setAlarmTime(std::mktime(newTime));
        }
    }

    void BellHomeScreenWindow::setSnoozeTime(std::time_t newTime)
    {
        if (currentLayout) {
            currentLayout->setSnoozeTime(newTime);
        }
    }

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
        application->switchWindow(gui::BellBatteryStatusWindow::name,
                                  std::make_unique<gui::BellBatteryStatusWindow::Data>(presenter->getBatteryLvl(),
                                                                                       presenter->isBatteryCharging()));
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
