// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Time.hpp"
#include "time/time_conversion.hpp"
#include "Style.hpp"

namespace gui::top_bar
{
    Time::Time(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h), _time{}
    {
        setFilled(false);
        setBorderColor(gui::ColorNoColor);
        setFont(style::header::status_bar::time::font);
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

    void Time::acceptStatusBarVisitor(StatusBarVisitor &visitor)
    {
        visitor.visit(*this);
    }

    TimeConfiguration::TimeConfiguration(TimeMode mode) : mode{mode}
    {}

    TimeConfiguration::TimeMode TimeConfiguration::getMode() const noexcept
    {
        return mode;
    }

    void TimeConfiguration::visit(gui::top_bar::Time &widget) const
    {
        using namespace utils::time;
        getMode() == TimeConfiguration::TimeMode::Time12h
            ? widget.setFormat(Locale::format(Locale::TimeFormat::FormatTime12H))
            : widget.setFormat(Locale::format(Locale::TimeFormat::FormatTime24H));
    }
} // namespace gui::top_bar
