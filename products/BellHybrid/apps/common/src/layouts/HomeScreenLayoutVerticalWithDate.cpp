// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutVerticalWithDate.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/ClockVertical.hpp>

namespace gui
{
    HomeScreenLayoutVerticalWithDate::HomeScreenLayoutVerticalWithDate(std::string name)
        : HomeScreenLayoutVertical(std::move(name))
    {
        using namespace bellMainStyle;

        const auto sideBoxWidth = (style::window_width - style::homescreen_vertical::center_box_w) / 2;

        leftBox->setMinimumSize(sideBoxWidth, style::window_height);
        leftBox->setMargins(Margins({0, 0, 0, 0}));
        leftBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Top));

        leftBox->addWidget(alarmMainIcon);
        alarmMainIcon->setMargins(Margins{0, style::homescreen_vertical_date::margin_top, 0, 0});
        alarmMainIcon->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Top));

        leftBox->addWidget(alarmMainTime);

        rightBox->setMinimumSize(sideBoxWidth, style::window_height);
        rightBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
        rightBox->setMargins(Margins({0, 0, 0, 0}));

        rightBox->addWidget(battery);
        battery->setFont(mainWindow::bottomDescription::font_smallnormal);
        battery->setBatteryPercentMode(BatteryPercentMode::Show);
        battery->setEdges(RectangleEdge::None);
        battery->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        battery->setMinimumSize(battery::battery_widget_w, style::homescreen_vertical::info_line_h);
        battery->setMaximumSize(battery::battery_widget_w, style::homescreen_vertical::info_line_h);

        date = new TextFixedSize(rightBox);
        date->setMaximumSize(style::bell_base_layout::outer_layouts_w, style::homescreen_vertical::info_line_h);
        date->setFont(mainWindow::bottomDescription::font_smallnormal);
        date->setEdges(RectangleEdge::None);
        date->activeItem = false;
        date->drawUnderline(false);
        date->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        date->setText("00/00");

        // Assume 12h format
        ampm = new TextFixedSize(rightBox);
        ampm->setMaximumSize(style::bell_base_layout::outer_layouts_w, style::homescreen_vertical::info_line_h);
        ampm->setFont(mainWindow::bottomDescription::font_smallnormal);
        ampm->setEdges(RectangleEdge::None);
        ampm->activeItem = false;
        ampm->drawUnderline(false);
        ampm->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        ampm->setText("XX");
        ampm->setMargins(Margins{0, 0, 0, style::homescreen_vertical_date::margin_bot});

        alarmMainIcon->informContentChanged();
        battery->informContentChanged();
        date->informContentChanged();
        ampm->informContentChanged();
    }

    auto HomeScreenLayoutVerticalWithDate::setTime(std::time_t newTime) -> void
    {
        HomeScreenLayoutVertical::setTime(newTime);

        const auto t = std::localtime(&newTime);

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << t->tm_mday;
        ss << '/';
        ss << std::setfill('0') << std::setw(2) << (t->tm_mon + 1);

        date->setText(ss.str());

        if (ampm->visible) {
            const auto hours = std::chrono::hours{t->tm_hour};
            const auto isPM  = date::is_pm(hours);
            ampm->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
        }
    }

    void HomeScreenLayoutVerticalWithDate::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        HomeScreenLayoutVertical::setTimeFormat(fmt);
        if (fmt == utils::time::Locale::TimeFormat::FormatTime12H) {
            ampm->setMargins(Margins{0, 0, 0, style::homescreen_vertical_date::margin_bot});
            date->setMargins({0, 0, 0, 0});
            ampm->setVisible(true);
        }
        else {
            ampm->setMargins({0, 0, 0, 0});
            date->setMargins(Margins{0, 0, 0, style::homescreen_vertical_date::margin_bot});
            ampm->setVisible(false);
        }
    }

    void HomeScreenLayoutVerticalWithDate::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        // In 24h mode battery indicator is lower so 100% is too long to be displayed
        if (!ampm->visible) {
            const auto percentMode = batteryContext.level < 100 ? BatteryPercentMode::Show : BatteryPercentMode::Hide;
            battery->setBatteryPercentMode(percentMode);
        }
        else
            battery->setBatteryPercentMode(BatteryPercentMode::Show);
        battery->update(batteryContext.level, isBatteryCharging(batteryContext.state));
        battery->setVisible(true);
        battery->informContentChanged();
    }

    bool HomeScreenLayoutVerticalWithDate::isAlarmTimeVisibilityAllowed()
    {
        return true;
    }
}; // namespace gui
