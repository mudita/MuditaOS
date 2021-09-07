// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Time.hpp"
#include "Style.hpp"
#include <time/time_conversion_factory.hpp>

#include <ctime>
namespace gui::status_bar
{
    Time::Time(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h), time{}
    {
        setEdges(RectangleEdge::None);
        setFont(style::status_bar::time::font);
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        using namespace utils::time;
        time = TimestampFactory().createTimestamp(TimestampType::Clock, std::time(nullptr));
        update();
    }

    void Time::update()
    {
        time->set_time(std::time(nullptr));
        setText(time->str());
    }
} // namespace gui::status_bar
