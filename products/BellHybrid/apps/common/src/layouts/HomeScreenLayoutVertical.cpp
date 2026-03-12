// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "layouts/HomeScreenLayoutVertical.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/SnoozeTimer.hpp"
#include "widgets/BellConnectionStatus.hpp"

#include <gui/widgets/Icon.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/ClockVertical.hpp>

namespace
{
    constexpr auto dischargingLevelShowTop{20};
} // namespace

namespace gui
{
    HomeScreenLayoutVertical::HomeScreenLayoutVertical(std::string name)
        : BaseHomeScreenLayoutProvider(std::move(name)),
          LayoutVertical(nullptr, 0, 0, style::window_width, style::window_height)
    {}

    auto HomeScreenLayoutVertical::getSnoozeTimer() -> SnoozeTimer *
    {
        return nullptr;
    }

    auto HomeScreenLayoutVertical::setViewState(app::home_screen::ViewState state) -> void
    {
        switch (state) {
        case app::home_screen::ViewState::Deactivated:
            alarmActivatedDeactivatedScreen->image->set("big_no-alarm_W_G", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            alarmMainIcon->setVisible(false);
            alarmMainTime->setVisible(false);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            setScreenMode(ScreenMode::Main);
            break;
        case app::home_screen::ViewState::DeactivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            alarmActivatedDeactivatedScreen->image->set("big_no-alarm_W_G", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            break;
        case app::home_screen::ViewState::WaitForConfirmation:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            break;
        case app::home_screen::ViewState::AlarmEdit:
            setScreenMode(ScreenMode::AlarmEdit);
            break;
        case app::home_screen::ViewState::ActivatedLowBattery:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            alarmActivatedDeactivatedScreen->image->set("bell_status_battery_lvl0", ImageTypeSpecifier::W_G);
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            break;
        case app::home_screen::ViewState::ActivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            alarmActivatedDeactivatedScreen->image->set("big_alarm_W_G", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            break;
        case app::home_screen::ViewState::Activated:
            alarmActivatedDeactivatedScreen->image->set("big_alarm_W_G", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            alarmMainIcon->setVisible(true);
            alarmMainIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            if (isAlarmTimeVisibilityAllowed()) {
                alarmMainTime->setVisible(true);
                // For unknown reason the mode is modified by showing/hiding windows
                alarmMainTime->setEditMode(EditMode::Browse);
            }
            else {
                alarmMainTime->setVisible(false);
            }
            alarmTopIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            setScreenMode(ScreenMode::Main);
            break;
        case app::home_screen::ViewState::AlarmRinging:
            setScreenMode(ScreenMode::Main);
            alarmMainIcon->setStatus(AlarmIcon::Status::RINGING);
            alarmMainTime->setVisible(false);
            break;
        case app::home_screen::ViewState::AlarmRingingDeactivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmActivatedDeactivatedScreen->image->set("big_no-alarm_W_G", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            break;
        case app::home_screen::ViewState::AlarmSnoozedWait:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            alarmMainTime->setVisible(false);
            alarmActivatedDeactivatedScreen->image->set("big_alarm_snoozed_W_M", {});
            alarmActivatedDeactivatedScreen->image->setMaximumWidth(alarmActivatedDeactivatedScreen->image->getWidth());
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            break;
        case app::home_screen::ViewState::AlarmSnoozed:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            alarmMainTime->setVisible(false);
            setScreenMode(ScreenMode::Main);
            break;
        case app::home_screen::ViewState::PreWakeUpActive:
            break;
        }
    }

    auto HomeScreenLayoutVertical::setScreenMode(ScreenMode mode) -> void
    {
        switch (mode) {
        case ScreenMode::Main:
            mainScreen->setVisible(true);
            setAlarmScreen->setVisible(false);
            alarmActivatedDeactivatedScreen->setVisible(false);
            mainScreen->informContentChanged();
            break;
        case ScreenMode::AlarmEdit:
            mainScreen->setVisible(false);
            setAlarmScreen->setVisible(true);
            alarmActivatedDeactivatedScreen->setVisible(false);
            setAlarmScreen->informContentChanged();
            // This is an ugly hack to update format on alarm spinner.
            // Will not be neccessary when visible flags get fixed
            setAlarmFmtSpinner->setTimeFormat(setAlarmFmtSpinner->getTimeFormat());
            break;
        case ScreenMode::AlarmActivatedDeactivated:
            mainScreen->setVisible(false);
            setAlarmScreen->setVisible(false);
            alarmActivatedDeactivatedScreen->setVisible(true);
            alarmActivatedDeactivatedScreen->informContentChanged();
            break;
        }
        resizeItems();
    }

    auto HomeScreenLayoutVertical::setTextDescription(const UTF8 &desc) -> void
    {
        alarmActivatedDeactivatedScreen->text->setRichText(desc);
        alarmActivatedDeactivatedScreen->text->informContentChanged();
    }

    auto HomeScreenLayoutVertical::isBatteryVisibilityAllowed(const Store::Battery &batteryContext) -> bool
    {
        return (batteryContext.level < dischargingLevelShowTop) || Store::Battery::isCharging();
    }

    auto HomeScreenLayoutVertical::isAlarmTimeVisibilityAllowed() -> bool
    {
        return false;
    }

    auto HomeScreenLayoutVertical::setBatteryLevelState(const Store::Battery &batteryContext) -> void
    {
        battery->update(batteryContext.level, Store::Battery::isCharging());
        battery->setVisible(isBatteryVisibilityAllowed(batteryContext));
        battery->informContentChanged();
    }

    auto HomeScreenLayoutVertical::setTime(std::time_t newTime) -> void
    {
        time->setTime(newTime);
    }

    auto HomeScreenLayoutVertical::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void
    {
        setAlarmFmtSpinner->setTimeFormat(fmt);
        alarmMainTime->setTimeFormat(fmt);
    }

    auto HomeScreenLayoutVertical::setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void
    {
        time->setTimeFormat(fmt);
    }

    auto HomeScreenLayoutVertical::getAlarmTime() const -> std::time_t
    {
        const auto now = std::time(nullptr);
        auto alarmTime = setAlarmFmtSpinner->getTime();
        if (alarmTime < now) {
            alarmTime += utils::time::secondsInDay;
        }
        return alarmTime;
    }

    auto HomeScreenLayoutVertical::setAlarmTime(std::time_t newTime) -> void
    {
        setAlarmFmtSpinner->setTime(newTime);
        alarmMainTime->setTime(newTime);
    }

    auto HomeScreenLayoutVertical::getLayout() -> Item *
    {
        return this;
    }

    auto HomeScreenLayoutVertical::updateUsbStatus(bool isConnected) -> void
    {
        connectionStatus->show(isConnected);
        connectionStatus->informContentChanged();
    }
} // namespace gui
