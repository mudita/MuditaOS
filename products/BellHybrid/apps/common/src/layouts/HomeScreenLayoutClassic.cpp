// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutClassic.hpp"
#include "layouts/HomeScreenLayoutNames.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/BellConnectionStatus.hpp"
#include "widgets/DuoHBox.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>

namespace
{
    constexpr auto dischargingLevelShowTop = 20;

    bool isBatteryCharging(const Store::Battery::State state)
    {
        using State = Store::Battery::State;
        return (state == State::Charging) || (state == State::ChargingDone);
    }
} // namespace

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

        timeHBox = new HBox(this->centerBox);
        timeHBox->setMaximumSize(style::bell_base_layout::center_layout_w, style::bell_base_layout::center_layout_h);
        timeHBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeHBox->setEdges(RectangleEdge::None);

        time = new TimeSetFmtSpinner(timeHBox);
        time->setMaximumSize(style::bell_base_layout::center_layout_w, style::bell_base_layout::center_layout_h);
        time->setFont(mainWindow::time::font);
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        time->setEditMode(EditMode::Browse);
        time->activeItem = false;

        statusBox = new HBox(this->lastBox);
        statusBox->setMinimumSize(style::homescreen_classic::status_box_layout_w,
                                  style::bell_base_layout::last_layout_h);
        statusBox->setEdges(RectangleEdge::None);
        statusBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        statusBox->setVisible(true);

        widgetBox = new VBox(statusBox);
        widgetBox->setMinimumSize(style::homescreen_classic::status_box_layout_w,
                                  style::bell_base_layout::last_layout_h);
        widgetBox->setEdges(RectangleEdge::None);
        widgetBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        widgetBox->setVisible(true);

        infoBox = new HBox(widgetBox);
        infoBox->setMinimumSize(style::homescreen_classic::status_box_layout_w,
                                style::homescreen_classic::info_box_layout_h);
        infoBox->setEdges(RectangleEdge::None);
        infoBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        infoBox->setVisible(true);

        connectionBox = new HBox(widgetBox);
        connectionBox->setMinimumSize(style::homescreen_classic::status_box_layout_w,
                                      style::homescreen_classic::connection_box_layout_h);
        connectionBox->setEdges(RectangleEdge::None);
        connectionBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        connectionBox->setVisible(true);

        battery = new BellBattery(infoBox, gui::BatteryWidthMode::FitToContent);
        battery->setMargins(Margins(0U, style::bell_base_layout::info_box_top_margin, 0U, 0U));
        battery->setMaximumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setEdges(RectangleEdge::None);
        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        battery->setVisible(false);
        battery->setBatteryPercentMode(BatteryPercentMode::Show);

        connectionStatus = new BellConnectionStatus(connectionBox);

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

    void HomeScreenLayoutClassic::setViewState(app::home_screen::ViewState state)
    {
        switch (state) {
        case app::home_screen::ViewState::Deactivated:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
            setHeaderViewMode(HeaderViewMode::Empty);
            alarm->setEditMode(EditMode::Browse);
            removeTextDescription();
            break;
        case app::home_screen::ViewState::DeactivatedWait:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            alarm->setEditMode(EditMode::Browse);
            break;
        case app::home_screen::ViewState::WaitForConfirmation:
            setTextDescription(utils::translate("app_bellmain_home_screen_bottom_desc_dp"));
            break;
        case app::home_screen::ViewState::AlarmEdit:
            alarm->setEditMode(EditMode::Edit);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            removeTextDescription();
            break;
        case app::home_screen::ViewState::ActivatedWait:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            alarm->setEditMode(EditMode::Browse);
            break;
        case app::home_screen::ViewState::Activated:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
            setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            alarm->setEditMode(EditMode::Browse);
            removeTextDescription();
            break;
        case app::home_screen::ViewState::AlarmRinging:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::RINGING);
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            alarm->setEditMode(EditMode::Browse);
            removeTextDescription();
            break;
        case app::home_screen::ViewState::AlarmRingingDeactivatedWait:
            alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
            setHeaderViewMode(HeaderViewMode::AlarmIcon);
            alarm->setEditMode(EditMode::Browse);
            break;
        case app::home_screen::ViewState::AlarmSnoozedWait:
            setHeaderViewMode(HeaderViewMode::SnoozeIcon);
            alarm->setEditMode(EditMode::Browse);
            break;
        case app::home_screen::ViewState::AlarmSnoozed:
            setHeaderViewMode(HeaderViewMode::SnoozeIconAndTime);
            alarm->setEditMode(EditMode::Browse);
            removeTextDescription();
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
            snoozeTimer->setTimerVisible(false);
            alarm->informContentChanged();
            break;
        case HeaderViewMode::AlarmIconAndTime:
            snoozeTimer->setVisible(false);
            snoozeTimer->informContentChanged();
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(true);
            alarm->informContentChanged();
            break;
        case HeaderViewMode::AlarmIcon:
            snoozeTimer->setVisible(false);
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(false);
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

    void HomeScreenLayoutClassic::setTextDescription(const UTF8 &desc)
    {
        statusBox->setVisible(false);
        bottomText->setVisible(true);
        bottomText->setRichText(desc);
        bottomText->informContentChanged();
    }

    void HomeScreenLayoutClassic::removeTextDescription()
    {
        bottomText->setText("");
        statusBox->setVisible(true);
        bottomText->setVisible(false);
        statusBox->resizeItems();
        statusBox->informContentChanged();
    }

    bool HomeScreenLayoutClassic::isBatteryVisibilityAllowed(const Store::Battery &batteryContext)
    {
        return (batteryContext.level < dischargingLevelShowTop) || isBatteryCharging(batteryContext.state);
    }

    void HomeScreenLayoutClassic::setBatteryLevelState(const Store::Battery &batteryContext)
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
        adjustConnectionStatusPosition();
    }

    void HomeScreenLayoutClassic::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(false);
    }

    void HomeScreenLayoutClassic::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
        // setTimeFormat makes AM/PM "visible" so disable it
        time->setTimeFormatSpinnerVisibility(false);
    }

    void HomeScreenLayoutClassic::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        alarm->setTimeFormat(fmt);
    }

    void HomeScreenLayoutClassic::setSnoozeFormat(utils::time::Locale::TimeFormat fmt)
    {
        snoozeTimer->setTimeFormat(fmt);
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
    auto HomeScreenLayoutClassic::setUSBStatusConnected() -> void
    {
        connectionStatus->setConnected();
        connectionStatus->informContentChanged();
    }

    auto HomeScreenLayoutClassic::adjustConnectionStatusPosition() -> void
    {
        if (getName() == gui::layout::Classic) {
            if (battery->visible) {
                widgetBox->removeWidget(infoBox);
                widgetBox->removeWidget(connectionBox);
                connectionBox->setMargins(Margins(0, 0, 0, 0));
                widgetBox->addWidget(infoBox);
                widgetBox->addWidget(connectionBox);
            }
            else {
                widgetBox->removeWidget(infoBox);
                widgetBox->removeWidget(connectionBox);
                connectionBox->setMargins(Margins(0,
                                                  style::homescreen_classic::connection_box_top_margin,
                                                  0,
                                                  style::homescreen_classic::connection_box_bottom_margin));
                widgetBox->addWidget(connectionBox);
                widgetBox->addWidget(infoBox);
            }
        }
    }
}; // namespace gui
