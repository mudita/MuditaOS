// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "widgets/LayoutVertical.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <common/data/StyleCommon.hpp>
#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetSpinnerVertical.hpp>

namespace gui
{
    LayoutVertical::LayoutVertical(
        Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : HBox(parent, x, y, w, h)
    {
        using namespace bellMainStyle;

        // Main Screen
        mainScreen = new HBox(this);
        mainScreen->setMinimumSize(style::window_width, style::window_height);

        leftBox = new VBox(mainScreen, 0, 0, 0, 0);
        leftBox->setMinimumSize(style::homescreen_vertical::side_box_w, style::window_height);
        leftBox->setEdges(RectangleEdge::None);

        auto centerBox = new VBox(mainScreen, 0, 0, 0, 0);
        centerBox->setMinimumSize(style::homescreen_vertical::center_box_w, style::window_height);
        centerBox->setEdges(RectangleEdge::None);

        time = new TimeSetSpinnerVertical(centerBox);
        time->setFont(mainWindow::time::font);
        time->setMinimumSize(style::homescreen_vertical::center_box_w, style::window_height);
        time->setEdges(RectangleEdge::None);
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        rightBox = new VBox(mainScreen, 0, 0, 0, 0);
        rightBox->setMinimumSize(style::homescreen_vertical::side_box_w, style::window_height);
        rightBox->setEdges(RectangleEdge::None);

        alarmMainIcon = new AlarmIcon(nullptr);
        alarmMainIcon->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                      style::bell_base_layout::outer_layouts_h);
        alarmMainIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
        alarmMainIcon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        battery = new BellBattery(nullptr, 0, 0, 0, 0);
        battery->setMinimumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setEdges(RectangleEdge::None);
        battery->setVisible(true);
        battery->setBatteryPercentMode(BatteryPercentMode::Hide);
        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        // Set Alarm Screen
        setAlarmScreen = new BellBaseLayout(this);
        setAlarmScreen->setMinimumSize(style::window_width, style::window_height);
        alarmTopIcon = new AlarmIcon(setAlarmScreen->firstBox);
        alarmTopIcon->setStatus(AlarmIcon::Status::DEACTIVATED);
        alarmTopIcon->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                     style::bell_base_layout::outer_layouts_h);

        alarmTopIcon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        setAlarmFmtSpinner = new TimeSetFmtSpinner(setAlarmScreen->getCenterBox());
        setAlarmFmtSpinner->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        setAlarmFmtSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setAlarmFmtSpinner->setFont(style::window::font::supersizemelight);
        setAlarmFmtSpinner->activeItem = false;

        setAlarmScreen->getCenterBox()->setAlignment(
            Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        setAlarmScreen->resizeItems();
        setAlarmScreen->setVisible(false);

        // Activation / Deactivation
        alarmActivatedDeactivatedScreen =
            new Icon(this, 0, 0, style::window_width, style::window_height, "big_alarm_W_G", {});
        alarmActivatedDeactivatedScreen->setMinimumSize(style::window_width, style::window_height);
        alarmActivatedDeactivatedScreen->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        alarmActivatedDeactivatedScreen->image->setMargins(
            {0, bell_style::popup_icon_top_margin, 0, bell_style::popup_icon_bottom_margin});
        alarmActivatedDeactivatedScreen->image->setAlignment(
            Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        alarmActivatedDeactivatedScreen->text->setFont(style::window::font::verybiglight);
        alarmActivatedDeactivatedScreen->resizeItems();
        alarmActivatedDeactivatedScreen->setVisible(false);
        alarmActivatedDeactivatedScreen->image->setEdges(RectangleEdge::All);

        resizeItems();
    }
}; // namespace gui
