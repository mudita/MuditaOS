// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/SnoozeTimer.hpp>
#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ImageBox.hpp>

namespace gui
{
    SnoozeTimer::SnoozeTimer(Item *parent, Position x, Position y, Length w, Length h) : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        auto alarmImg = new ImageBox(this, 0, 0, 0, 0, new Image("bell_alarm_snooze_W_M"));
        alarmImg->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        alarmImg->setMargins(Margins(10U, 0, 10U, 0));
        alarmImg->setMinimumSizeToFitImage();

        timeSpinner = new TimeSetFmtSpinner(this);
        timeSpinner->setFont(style::window::font::largelight);
        timeSpinner->setEditMode(EditMode::Browse);
        timeSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeSpinner->setMargins(Margins(0, 0, 10U, 0));
        timeSpinner->setEdges(RectangleEdge::None);
        timeSpinner->setFocusItem(nullptr);

        resizeItems();
    }

    auto SnoozeTimer::setFont(std::string newFontName) noexcept -> void
    {
        fontName = std::move(newFontName);
        timeSpinner->setFont(fontName);
    }

    auto SnoozeTimer::setTime(std::uint8_t mins, std::uint8_t secs) noexcept -> void
    {
        timeSpinner->setHour(mins);
        timeSpinner->setMinute(secs);
    }
    auto SnoozeTimer::setTime(std::time_t time) noexcept -> void
    {
        timeSpinner->setTime(time);
    }

    auto SnoozeTimer::setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void
    {
        timeSpinner->setTimeFormat(fmt);
    }

    auto SnoozeTimer::setTimerVisible(bool visible) noexcept -> void
    {
        timeSpinner->setVisible(visible);
        timeSpinner->setFocusItem(nullptr);
        timeSpinner->informContentChanged();
    }

} /* namespace gui */
