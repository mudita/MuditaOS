// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassic.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    HomeScreenLayoutClassic::HomeScreenLayoutClassic(std::string name)
        : BaseHomeScreenLayoutProvider(std::move(name)),
          BellBaseLayout(nullptr, 0, 0, style::window_width, style::window_height, false)
    {
        buildInterface();
    }

    void HomeScreenLayoutClassic::buildInterface()
    {
        using namespace bellMainStyle;

        alarm = new AlarmSetSpinner(this->firstBox);
        alarm->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        alarm->setFont(mainWindow::alarmSetSpinner::font);
        alarm->setEditMode(EditMode::Browse);
        alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
        alarm->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        snoozeTimer = new SnoozeTimer(this->firstBox);
        snoozeTimer->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        snoozeTimer->setVisible(false);
        snoozeTimer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        time = new TimeSetFmtSpinner(this->centerBox);
        time->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        time->setFont(bellMainStyle::mainWindow::time::font);
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        time->setEditMode(EditMode::Browse);
        time->setFont(mainWindow::time::font);
        time->activeItem = false;

        statusBox = new DuoHBox(this->lastBox, 0, 0, 0, 0);
        statusBox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        statusBox->setEdges(RectangleEdge::None);
        statusBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        statusBox->setVisible(true);

        battery = new BellBattery(nullptr, 0, 0, 0, 0);
        battery->setMinimumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setEdges(RectangleEdge::None);
        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        battery->setVisible(true);
        battery->setBatteryPercentMode(BatteryPercentMode::Static);

        statusBox->setItems(battery, nullptr);

        bottomText = new TextFixedSize(this->lastBox, 0, 0, 0, 0);
        bottomText->setMaximumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        bottomText->setFont(mainWindow::bottomDescription::font_small);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->drawUnderline(false);
        bottomText->setVisible(false);

        resizeItems();
    }

    SnoozeTimer *HomeScreenLayoutClassic::getSnoozeTimer()
    {
        return snoozeTimer;
    }

    void HomeScreenLayoutClassic::setAlarmTriggered()
    {
        alarm->setAlarmStatus(AlarmSetSpinner::Status::RINGING);
    }

    void HomeScreenLayoutClassic::setAlarmActive(bool val)
    {
        if (val) {
            alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
        }
        else {
            alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
        }
    }

    void HomeScreenLayoutClassic::setViewState(app::home_screen::ViewState state)
    {
        switch (state) {
        case app::home_screen::ViewState::Deactivated:
            setAlarmEdit(false);
            setAlarmActive(false);
            setHeaderViewMode(HeaderViewMode::Empty);
            break;
        case app::home_screen::ViewState::DeactivatedWait:
            setAlarmActive(false);
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            break;
        case app::home_screen::ViewState::WaitForConfirmation:
            setBottomDescription(utils::translate("app_bellmain_home_screen_bottom_desc_dp"));
            break;
        case app::home_screen::ViewState::AlarmEdit:
            setAlarmEdit(true);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            break;
        case app::home_screen::ViewState::ActivatedWait:
            setAlarmActive(true);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            break;
        case app::home_screen::ViewState::Activated:
            setAlarmActive(true);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            break;
        case app::home_screen::ViewState::AlarmRinging:
            setAlarmTriggered();
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            break;
        case app::home_screen::ViewState::AlarmRingingDeactivatedWait:
            setAlarmActive(false);
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            break;
        case app::home_screen::ViewState::AlarmSnoozedWait:
            setHeaderViewMode(HeaderViewMode::SnoozeIcon);
            break;
        case app::home_screen::ViewState::AlarmSnoozed:
            setHeaderViewMode(HeaderViewMode::SnoozeIconAndTime);
            break;
        }
    }

    void HomeScreenLayoutClassic::setHeaderViewMode(HeaderViewMode mode)
    {
        switch (mode) {
        case HeaderViewMode::Empty:
            alarm->setVisible(false);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case HeaderViewMode::AlarmIconAndTime:
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(true);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case HeaderViewMode::AlarmIcon:
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case HeaderViewMode::SnoozeIconAndTime:
            alarm->setVisible(false);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(true);
            snoozeTimer->setTimerVisible(true);
            snoozeTimer->informContentChanged();
            break;
        case HeaderViewMode::SnoozeIcon:
            alarm->setVisible(false);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(true);
            snoozeTimer->informContentChanged();
            snoozeTimer->setTimerVisible(false);
            break;
        }
    }

    void HomeScreenLayoutClassic::setTemperature(utils::temperature::Temperature newTemp)
    {}

    void HomeScreenLayoutClassic::setBottomDescription(const UTF8 &desc)
    {
        statusBox->setVisible(false);
        bottomText->setVisible(true);
        bottomText->setRichText(desc);
        bottomText->informContentChanged();
    }

    void HomeScreenLayoutClassic::removeBottomDescription()
    {
        bottomText->setText("");
        statusBox->setVisible(true);
        bottomText->setVisible(false);
        statusBox->resizeItems();
        statusBox->informContentChanged();
    }

    bool HomeScreenLayoutClassic::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        // TODO fix magic values
        return (batteryContext.level < battery::dischargingLevelShowTop) ||
               (batteryContext.state == Store::Battery::State::Charging &&
                batteryContext.level != battery::chargingLevelHideBot);
    }

    void HomeScreenLayoutClassic::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        battery->update(batteryContext);
        if (isBatteryVisibilityAllowed(batteryContext)) {
            battery->setVisible(true);
        }
        else {
            battery->setVisible(false);
        }
        battery->informContentChanged();
    }

    void HomeScreenLayoutClassic::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(false);
    }

    void HomeScreenLayoutClassic::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    void HomeScreenLayoutClassic::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        alarm->setTimeFormat(fmt);
    }

    void HomeScreenLayoutClassic::setSnoozeFormat(utils::time::Locale::TimeFormat fmt)
    {
        snoozeTimer->setTimeFormat(fmt);
    }

    void HomeScreenLayoutClassic::setAlarmEdit(bool val)
    {
        if (not val) {
            alarm->setEditMode(EditMode::Browse);
        }
        else {
            alarm->setEditMode(EditMode::Edit);
        };
    }

    std::time_t HomeScreenLayoutClassic::getAlarmTime() const
    {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto alarmTime = alarm->getTime();
        if (alarmTime < now) {
            alarmTime += utils::time::secondsInDay;
        }
        return alarmTime;
    }

    void HomeScreenLayoutClassic::setAlarmTime(std::time_t newTime)
    {
        alarm->setTime(newTime);
    }

    void HomeScreenLayoutClassic::setSnoozeTime(std::time_t newTime)
    {
        snoozeTimer->setTime(newTime);
    }

    auto HomeScreenLayoutClassic::getLayout() -> Item *
    {
        return this;
    }
}; // namespace gui
