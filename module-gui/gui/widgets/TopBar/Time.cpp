// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Time.hpp"
#include "time/time_conversion.hpp"
#include <Utils.hpp>
#include <Style.hpp>

namespace gui::top_bar
{
    Time::Time(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h), _time{}
    {
        setFilled(false);
        setBorderColor(gui::ColorNoColor);
        setFont(style::header::font::time);
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        using namespace utils::time;
        setFormat(Locale::format(Locale::TimeFormat::FormatTime12H));
        update();
    }

    void Time::update()
    {
        _time.set_time(utils::time::getCurrentTimestamp().getTime());
        setText(_time.str());
    }

    void Time::setFormat(const std::string &format)
    {
        _time.set_format(format);
    }
} // namespace gui::top_bar
