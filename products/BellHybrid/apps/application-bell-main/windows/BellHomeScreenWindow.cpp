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

        if (auto snoozeTimer = currentLayout->getSnoozeTimer()) {
            auto timer = std::make_unique<app::ProgressTimerWithSnoozeTimer>(
                application, *this, snoozeTimerName, timerTick, app::ProgressCountdownMode::Increasing);
            timer->attach(snoozeTimer);
            presenter->setSnoozeTimer(std::move(timer));
        }
    }

    void BellHomeScreenWindow::setViewState(app::home_screen::ViewState state)
    {
        if (currentLayout) {
            currentLayout->setViewState(state);
            currentLayout->getLayout()->setVisible(true);
        }
    }

    void BellHomeScreenWindow::setTemperature(utils::temperature::Temperature newTemp)
    {
        if (currentLayout) {
            currentLayout->setTemperature(newTemp);
        }
    }

    void BellHomeScreenWindow::setTextDescription(const UTF8 &desc)
    {
        if (currentLayout) {
            currentLayout->setTextDescription(desc);
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

    void BellHomeScreenWindow::setSnoozeTime(std::time_t newTime)
    {
        if (currentLayout) {
            currentLayout->setSnoozeTime(newTime);
        }
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
        presenter->onBeforeShow();
        // Show the screen only on state change
        // to avoid inital state homescreen display on powerup
        currentLayout->getLayout()->setVisible(false);

        const auto alarmRingingSwitchData = dynamic_cast<app::actions::AlarmRingingData *>(data);
        if (alarmRingingSwitchData != nullptr) {
            presenter->handleAlarmRingingEvent();
        }
    }
    RefreshModes BellHomeScreenWindow::updateTime()
    {
        if (presenter) {
            return presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_NONE;
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
