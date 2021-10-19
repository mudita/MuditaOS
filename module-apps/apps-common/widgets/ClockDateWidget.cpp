// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ClockDateWidget.hpp"
#include <Style.hpp>
#include <time/time_conversion_factory.hpp>
#include <api/TimeSettingsApi.hpp>
#include <date/date.h>

namespace gui
{
    ClockDateWidget::ClockDateWidget(Item *parent, Position x, Position y, Length w, Length h)
        : VBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        setEdges(RectangleEdge::None);

        clock = new gui::TimeSetSpinner(this);
        clock->setFont(style::window::font::supersizemelight);
        clock->setEdges(RectangleEdge::None);

        date = new gui::Label(this);
        date->setMargins({0, clock_date_widget::day_top_margin, 0, 0});
        date->setMinimumSize(style::window_width, clock_date_widget::day_h);
        date->setFont(style::window::font::biglight);
        date->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        date->setEdges(RectangleEdge::None);
    }

    void ClockDateWidget::setTime(std::time_t time)
    {
        if (!stm::api::isTimeFormat12h()) {
            clock->setTime(time);
        }
        else {
            const auto t = std::localtime(&time);
            clock->setTime(date::make12(std::chrono::hours{t->tm_hour}).count(), t->tm_min);
        }

        date->setText(
            utils::time::TimestampFactory().createTimestamp(utils::time::TimestampType::DateText, time)->str());
    }
} // namespace gui
