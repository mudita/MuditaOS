// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutVerticalWithAmPm.hpp"
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
#include <widgets/TimeSetSpinnerVertical.hpp>

namespace gui
{
    HomeScreenLayoutVerticalWithAmPm::HomeScreenLayoutVerticalWithAmPm(std::string name)
        : HomeScreenLayoutVertical(std::move(name))
    {
        using namespace bellMainStyle;

        leftBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        auto leftTopBox = new gui::VBox(leftBox);
        leftTopBox->setMinimumSize(leftTopBox->parent->getWidth(), leftTopBox->parent->getHeight() / 2);
        leftTopBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        leftTopBox->setEdges(RectangleEdge::None);

        auto leftBottomBox = new gui::VBox(leftBox);
        leftBottomBox->setMinimumSize(leftBottomBox->parent->getWidth(), leftBottomBox->parent->getHeight() / 2);
        leftBottomBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        leftBottomBox->setEdges(RectangleEdge::None);

        leftTopBox->addWidget(alarmMainIcon);
        leftTopBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        alarmMainIcon->setMargins(Margins{0, style::homescreen_vertical_ampm::alarm_margin_top, 0, 0});
        alarmMainIcon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        leftBottomBox->addWidget(battery);
        leftBottomBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        battery->setMargins(Margins{0, 0, 0, style::homescreen_vertical_ampm::battery_margin_bot});

        rightBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Bottom));
        fmt = new TextFixedSize(rightBox, 0, 0, 0, 0);
        fmt->setMaximumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        fmt->setFont(mainWindow::bottomDescription::font_normal);
        fmt->setEdges(RectangleEdge::None);
        fmt->activeItem = false;
        fmt->drawUnderline(false);
        fmt->setMargins(Margins{0, 0, 0, style::homescreen_vertical_ampm::fmt_margin_bot});
        fmt->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));

        alarmMainIcon->informContentChanged();
        battery->informContentChanged();
        fmt->informContentChanged();
    }

    auto HomeScreenLayoutVerticalWithAmPm::setTime(std::time_t newTime) -> void
    {
        HomeScreenLayoutVertical::setTime(newTime);

        const auto t     = std::localtime(&newTime);
        const auto hours = std::chrono::hours{t->tm_hour};
        const auto isPM  = date::is_pm(hours);
        fmt->setText(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
    }
}; // namespace gui
