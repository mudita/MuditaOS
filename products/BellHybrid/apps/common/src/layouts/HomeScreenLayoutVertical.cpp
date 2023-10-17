// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutVertical.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"
#include "widgets/SnoozeTimer.hpp"
#include "widgets/BellConnectionStatus.hpp"

#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/widgets/Icon.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/ClockVertical.hpp>

namespace
{
    constexpr auto dischargingLevelShowTop = 20;
} // namespace

namespace gui
{
    HomeScreenLayoutVertical::HomeScreenLayoutVertical(std::string name)
        : BaseHomeScreenLayoutProvider(std::move(name)),
          LayoutVertical(nullptr, 0, 0, style::window_width, style::window_height)
    {}

    SnoozeTimer *HomeScreenLayoutVertical::getSnoozeTimer()
    {
        return nullptr;
    }

    void HomeScreenLayoutVertical::setViewState(app::home_screen::ViewState state)
    {
        switch (state) {
        case app::home_screen::ViewState::Deactivated:
            alarmActivatedDeactivatedScreen->image->set("big_no-alarm_W_G", {});
            alarmMainIcon->setVisible(false);
            alarmMainTime->setVisible(false);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            setScreenMode(ScreenMode::Main);
            break;
        case app::home_screen::ViewState::DeactivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            alarmActivatedDeactivatedScreen->image->set("big_no-alarm_W_G", {});
            break;
        case app::home_screen::ViewState::WaitForConfirmation:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            break;
        case app::home_screen::ViewState::AlarmEdit:
            setScreenMode(ScreenMode::AlarmEdit);
            break;
        case app::home_screen::ViewState::ActivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            alarmActivatedDeactivatedScreen->image->set("big_alarm_W_G", {});
            break;
        case app::home_screen::ViewState::Activated:
            alarmActivatedDeactivatedScreen->image->set("big_alarm_W_G", {});
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
            break;
        case app::home_screen::ViewState::AlarmSnoozedWait:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            alarmMainTime->setVisible(false);
            alarmActivatedDeactivatedScreen->image->set("big_alarm_snoozed_W_M", {});
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            break;
        case app::home_screen::ViewState::AlarmSnoozed:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            alarmMainTime->setVisible(false);
            setScreenMode(ScreenMode::Main);
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

    void HomeScreenLayoutVertical::setTextDescription(const UTF8 &desc)
    {
        alarmActivatedDeactivatedScreen->text->setRichText(desc);
        alarmActivatedDeactivatedScreen->text->informContentChanged();
    }

    bool HomeScreenLayoutVertical::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return (batteryContext.level < dischargingLevelShowTop) || isBatteryCharging(batteryContext.state);
    }

    bool HomeScreenLayoutVertical::isAlarmTimeVisibilityAllowed()
    {
        return false;
    }

    bool HomeScreenLayoutVertical::isBatteryCharging(const Store::Battery::State state)
    {
        using State = Store::Battery::State;
        return (state == State::Charging) || (state == State::ChargingDone);
    }

    void HomeScreenLayoutVertical::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        battery->update(batteryContext.level, isBatteryCharging(batteryContext.state));
        connectionStatus->checkIfConnected(batteryContext.state);

        if (isBatteryVisibilityAllowed(batteryContext)) {
            battery->setVisible(true);
        }
        else {
            battery->setVisible(false);
        }
        battery->informContentChanged();
        connectionStatus->informContentChanged();
    }

    void HomeScreenLayoutVertical::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
    }

    void HomeScreenLayoutVertical::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        setAlarmFmtSpinner->setTimeFormat(fmt);
        alarmMainTime->setTimeFormat(fmt);
    }

    void HomeScreenLayoutVertical::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    std::time_t HomeScreenLayoutVertical::getAlarmTime() const
    {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto alarmTime = setAlarmFmtSpinner->getTime();
        if (alarmTime < now) {
            alarmTime += utils::time::secondsInDay;
        }
        return alarmTime;
    }

    void HomeScreenLayoutVertical::setAlarmTime(std::time_t newTime)
    {
        setAlarmFmtSpinner->setTime(newTime);
        alarmMainTime->setTime(newTime);
    }

    auto HomeScreenLayoutVertical::getLayout() -> Item *
    {
        return this;
    }
    auto HomeScreenLayoutVertical::setUSBStatusConnected() -> void
    {
        connectionStatus->setConnected();
        connectionStatus->informContentChanged();
    }

}; // namespace gui
