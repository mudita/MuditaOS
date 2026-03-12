// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <layouts/HomeScreenLayoutBinary.hpp>

#include <widgets/ClockBinary.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/BellBattery.hpp>
#include <widgets/BellConnectionStatus.hpp>
#include <widgets/BellBaseLayout.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>
#include <widgets/Icon.hpp>

#include <data/StyleCommon.hpp>

#include <time/time_constants.hpp>

namespace
{
    namespace top_box
    {
        constexpr auto height{style::window_height - 144};
    }

    namespace center_box
    {
        constexpr auto height{64};
        constexpr auto topMargin{14};
    } // namespace center_box

    namespace bottom_box
    {
        constexpr auto height{56};
    }

    namespace battery_widget
    {
        constexpr auto leftMargin{122};
    }

    namespace alarm_main_icon
    {
        constexpr auto leftMargin{119};
    }

    namespace clock
    {
        constexpr auto width{style::window_width - 230};
        constexpr auto height{top_box::height};
    } // namespace clock
} // namespace

namespace gui
{
    HomeScreenLayoutBinary::HomeScreenLayoutBinary(std::string name)
        : BaseHomeScreenLayoutProvider(std::move(name)), HBox(nullptr, 0, 0, style::window_width, style::window_height)
    {
        setEdges(RectangleEdge::None);

        mainScreen = new VBox(this);
        mainScreen->setMinimumSize(style::window_width, style::window_height);
        mainScreen->setEdges(RectangleEdge::None);

        topBox = new HBox(mainScreen);
        topBox->setMinimumSize(style::window_width, top_box::height);
        topBox->setMaximumHeight(top_box::height);
        topBox->setEdges(RectangleEdge::None);
        topBox->setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Center});
        topBox->activeItem = false;

        clock = new ClockBinary(topBox);
        clock->setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Bottom});
        clock->setMinimumSize(clock::width, clock::height);
        clock->setEdges(RectangleEdge::Bottom);
        clock->setVisible(true);

        centerBox = new HBox(mainScreen);
        centerBox->setMinimumSize(style::window_width, center_box::height);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment({Alignment::Horizontal::Left, Alignment::Vertical::Center});
        centerBox->setMargins({0, center_box::topMargin, 0, 0});
        centerBox->setVisible(true);
        centerBox->activeItem = false;

        battery = new BellBattery(centerBox, BatteryWidthMode::Fixed);
        battery->setMinimumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setBatteryPercentMode(BatteryPercentMode::Show);
        battery->setEdges(RectangleEdge::None);
        battery->setMargins({battery_widget::leftMargin, 0, 0, 0});
        battery->setVisible(true);

        alarmMainIcon = new AlarmIcon(centerBox);
        alarmMainIcon->setMargins({alarm_main_icon::leftMargin, 0, 0, 0});
        alarmMainIcon->setStatus(AlarmIcon::Status::ACTIVATED);
        alarmMainIcon->setVisible(true);

        setAlarmScreen = new BellBaseLayout(this);
        setAlarmScreen->setMinimumSize(style::window_width, style::window_height);

        alarmTopIcon = new AlarmIcon(setAlarmScreen->firstBox);
        alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
        alarmTopIcon->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                     style::bell_base_layout::outer_layouts_h);

        setAlarmFmtSpinner = new TimeSetFmtSpinner(setAlarmScreen->getCenterBox());
        setAlarmFmtSpinner->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        setAlarmFmtSpinner->setFont(style::window::font::supersizemelight);
        setAlarmFmtSpinner->activeItem = false;

        setAlarmScreen->getCenterBox()->setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Center});
        setAlarmScreen->resizeItems();
        setAlarmScreen->setVisible(false);

        alarmActivationScreen = new Icon(this,
                                         0,
                                         0,
                                         style::window_width,
                                         style::window_height,
                                         "bell_status_battery_lvl0",
                                         {},
                                         gui::ImageTypeSpecifier::W_G);
        alarmActivationScreen->setMinimumSize(style::window_width, style::window_height);
        alarmActivationScreen->setAlignment({gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top});
        alarmActivationScreen->image->setMargins({0, bell_style::warning_icon_top_margin, 0, 0});
        alarmActivationScreen->image->setEdges(RectangleEdge::None);
        alarmActivationScreen->text->setMaximumWidth(bell_style::warning_text_width);
        alarmActivationScreen->text->setFont(style::window::font::verybiglight);
        alarmActivationScreen->resizeItems();
        alarmActivationScreen->setVisible(false);

        bottomBox = new HBox(mainScreen);
        bottomBox->setMinimumSize(style::window_width, bottom_box::height);
        bottomBox->setEdges(RectangleEdge::None);
        bottomBox->setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Top});
        bottomBox->setVisible(true);
        bottomBox->activeItem = false;

        connectionStatus = new BellConnectionStatus(bottomBox);

        resizeItems();
    }

    auto HomeScreenLayoutBinary::setViewState(app::home_screen::ViewState state) -> void
    {
        switch (state) {
        case app::home_screen::ViewState::Deactivated:
            setAlarmActivationImage("big_no-alarm_W_G", {});
            alarmMainIcon->setVisible(false);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            setScreenMode(ScreenMode::Main);
            break;

        case app::home_screen::ViewState::DeactivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
            setAlarmActivationImage("big_no-alarm_W_G", {});
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
            setAlarmActivationImage("bell_status_battery_lvl0", ImageTypeSpecifier::W_G);
            break;

        case app::home_screen::ViewState::ActivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            alarmTopIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            setAlarmActivationImage("big_alarm_W_G", {});
            break;

        case app::home_screen::ViewState::Activated:
            setAlarmActivationImage("big_alarm_W_G", {});
            alarmMainIcon->setVisible(true);
            alarmMainIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            alarmTopIcon->setStatus(AlarmIcon::Status::ACTIVATED);
            setScreenMode(ScreenMode::Main);
            break;

        case app::home_screen::ViewState::AlarmRinging:
            setScreenMode(ScreenMode::Main);
            alarmMainIcon->setStatus(AlarmIcon::Status::RINGING);
            break;

        case app::home_screen::ViewState::AlarmRingingDeactivatedWait:
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            setAlarmActivationImage("big_no-alarm_W_G", {});
            break;

        case app::home_screen::ViewState::AlarmSnoozedWait:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            setAlarmActivationImage("big_alarm_snoozed_W_M", {});
            setScreenMode(ScreenMode::AlarmActivatedDeactivated);
            break;

        case app::home_screen::ViewState::AlarmSnoozed:
            alarmMainIcon->setStatus(AlarmIcon::Status::SNOOZE);
            setScreenMode(ScreenMode::Main);
            break;

        case app::home_screen::ViewState::PreWakeUpActive:
            break;
        }
    }

    auto HomeScreenLayoutBinary::setTextDescription(const UTF8 &desc) -> void
    {
        alarmActivationScreen->text->setRichText(desc);
        alarmActivationScreen->text->informContentChanged();
    }

    auto HomeScreenLayoutBinary::setBatteryLevelState(const Store::Battery &batteryContext) -> void
    {
        battery->update(batteryContext.level, Store::Battery::isCharging());
        battery->informContentChanged();
    }

    auto HomeScreenLayoutBinary::updateUsbStatus(bool isConnected) -> void
    {
        connectionStatus->show(isConnected);
        connectionStatus->informContentChanged();
    }

    auto HomeScreenLayoutBinary::setTime(std::time_t newTime) -> void
    {
        clock->setTime(newTime);
    }

    auto HomeScreenLayoutBinary::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void
    {
        setAlarmFmtSpinner->setTimeFormat(fmt);
    }

    auto HomeScreenLayoutBinary::getAlarmTime() const -> std::time_t
    {
        const auto now = std::time(nullptr);
        auto alarmTime = setAlarmFmtSpinner->getTime();
        if (alarmTime < now) {
            alarmTime += utils::time::secondsInDay;
        }
        return alarmTime;
    }

    auto HomeScreenLayoutBinary::setAlarmTime(std::time_t newTime) -> void
    {
        setAlarmFmtSpinner->setTime(newTime);
    }

    auto HomeScreenLayoutBinary::getSnoozeTimer() -> SnoozeTimer *
    {
        return nullptr;
    }

    auto HomeScreenLayoutBinary::getLayout() -> Item *
    {
        return this;
    }

    auto HomeScreenLayoutBinary::setScreenMode(ScreenMode mode) -> void
    {
        switch (mode) {
        case ScreenMode::Main:
            mainScreen->setVisible(true);
            setAlarmScreen->setVisible(false);
            alarmActivationScreen->setVisible(false);
            mainScreen->informContentChanged();
            break;

        case ScreenMode::AlarmEdit:
            mainScreen->setVisible(false);
            setAlarmScreen->setVisible(true);
            alarmActivationScreen->setVisible(false);
            setAlarmScreen->informContentChanged();
            /* Ugly hack to update format on alarm spinner, won't be needed when visible flags get fixed (probably
             * never) */
            setAlarmFmtSpinner->setTimeFormat(setAlarmFmtSpinner->getTimeFormat());
            break;

        case ScreenMode::AlarmActivatedDeactivated:
            mainScreen->setVisible(false);
            setAlarmScreen->setVisible(false);
            alarmActivationScreen->setVisible(true);
            alarmActivationScreen->informContentChanged();
            break;
        }
        resizeItems();
    }

    auto HomeScreenLayoutBinary::setAlarmActivationImage(const UTF8 &name, ImageTypeSpecifier specifier) -> void
    {
        alarmActivationScreen->image->set(name, specifier);
        alarmActivationScreen->image->setMaximumWidth(alarmActivationScreen->image->getWidth());
    }
} // namespace gui
