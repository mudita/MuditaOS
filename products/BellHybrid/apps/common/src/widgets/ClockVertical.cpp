// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/ClockVertical.hpp>
#include "widgets/LayoutVertical.hpp"

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <widgets/spinners/Spinners.hpp>

static constexpr auto doubleDigit = 10;

namespace gui
{

    ClockVertical::ClockVertical(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : VBox{parent, x, y, w, h}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hBoxHours = new gui::HBox(this, 0, 0, style::homescreen_vertical::center_box_w, 0);
        hBoxHours->setEdges(gui::RectangleEdge::None);
        hBoxHours->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        hBoxHours->setMinimumSize(style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);
        hBoxHours->setMargins(gui::Margins(0, 0, 0, style::homescreen_vertical::topNegativeMargin));

        hBoxHoursLeft = new gui::HBox(hBoxHours, 0, 0, 0, 0);
        hBoxHoursLeft->setEdges(gui::RectangleEdge::None);
        hBoxHoursLeft->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        hBoxHoursLeft->setMaximumSize(style::homescreen_vertical::digit_box_w,
                                      style::homescreen_vertical::center_box_h);

        hBoxHoursRight = new gui::HBox(hBoxHours, 0, 0, 0, 0);
        hBoxHoursRight->setEdges(gui::RectangleEdge::None);
        hBoxHoursRight->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        hBoxHoursRight->setMaximumSize(style::homescreen_vertical::digit_box_w,
                                       style::homescreen_vertical::center_box_h);

        hourFirst = new gui::TextFixedSize(hBoxHoursLeft, 0, 0, 0, 0);
        updateFont(hourFirst, fontName);

        hourFirst->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        hourFirst->setEdges(RectangleEdge::None);
        hourFirst->setText("0");
        hourFirst->drawUnderline(false);

        hourSecond = new gui::TextFixedSize(hBoxHoursRight, 0, 0, 0, 0);
        updateFont(hourSecond, fontName);

        hourSecond->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        hourSecond->setEdges(RectangleEdge::None);
        hourSecond->setText("0");
        hourSecond->drawUnderline(false);

        hBoxMinutes = new gui::HBox(this, 0, 0, style::homescreen_vertical::center_box_w, 0);
        hBoxMinutes->setEdges(gui::RectangleEdge::None);
        hBoxMinutes->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        hBoxMinutes->setMargins(gui::Margins(0, 0, 0, 0));
        hBoxMinutes->setMinimumSize(style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);

        hBoxMinutesLeft = new gui::HBox(hBoxMinutes, 0, 0, 0, 0);
        hBoxMinutesLeft->setEdges(gui::RectangleEdge::None);
        hBoxMinutesLeft->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        hBoxMinutesLeft->setMaximumSize(style::homescreen_vertical::digit_box_w,
                                        style::homescreen_vertical::center_box_h);

        hBoxMinutesRight = new gui::HBox(hBoxMinutes, 0, 0, 0, 0);
        hBoxMinutesRight->setEdges(gui::RectangleEdge::None);
        hBoxMinutesRight->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        hBoxMinutesRight->setMaximumSize(style::homescreen_vertical::digit_box_w,
                                         style::homescreen_vertical::center_box_h);

        minuteFirst = new gui::TextFixedSize(hBoxMinutesLeft, 0, 0, 0, 0);
        updateFont(minuteFirst, fontName);

        minuteFirst->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        minuteFirst->setEdges(RectangleEdge::None);
        minuteFirst->setText("0");
        minuteFirst->drawUnderline(false);

        minuteSecond = new gui::TextFixedSize(hBoxMinutesRight, 0, 0, 0, 0);
        updateFont(minuteSecond, fontName);

        minuteSecond->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        minuteSecond->setEdges(RectangleEdge::None);
        minuteSecond->setText("0");
        minuteSecond->drawUnderline(false);

        resizeItems();

        setTimeFormat(timeFormat);
    }

    auto ClockVertical::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        using namespace utils;

        const auto hourFirstStr  = hourFirst->getText();
        const auto hourSecondStr = hourSecond->getText();
        const auto hoursStr      = hourFirstStr + hourSecondStr;
        const auto hoursInt      = std::stoi(hoursStr.c_str());
        const auto hours         = std::chrono::hours(hoursInt);

        timeFormat = newFormat;

        switch (newFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            if (timeFormat != newFormat) {
                const auto time12H = date::make12(hours);
                setHour(time12H.count());
            }

        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            if (newFormat != timeFormat) {
                const auto time24H = date::make24(hours, isPM());
                setHour(time24H.count());
            }
        } break;
        default:
            break;
        }

        timeFormat = newFormat;
    }

    auto ClockVertical::setMinute(int value) noexcept -> void
    {
        const auto valueStr = std::to_string(value);
        if (value < doubleDigit) {
            minuteFirst->setText("0");
            minuteSecond->setText(valueStr.substr(0, 1));
        }
        else {
            minuteFirst->setText(valueStr.substr(0, 1));
            minuteSecond->setText(valueStr.substr(1, 1));
        }
    }

    auto ClockVertical::getHour() const noexcept -> int
    {
        auto hourFirstStr  = hourFirst->getText();
        auto hourSecondStr = hourSecond->getText();
        auto hoursStr      = hourFirstStr + hourSecondStr;
        return std::stoi(hoursStr.c_str());
    }

    auto ClockVertical::getMinute() const noexcept -> int
    {
        auto minuteFirstStr  = minuteFirst->getText();
        auto minuteSecondStr = minuteSecond->getText();
        auto minutesStr      = minuteFirstStr + minuteSecondStr;
        return std::stoi(minutesStr.c_str());
    }

    auto ClockVertical::setHour(int value) noexcept -> void
    {
        auto valueStr = std::to_string(value);
        if (value < doubleDigit) {
            hourFirst->setText("0");
            hourSecond->setText(valueStr.substr(0, 1));
        }
        else {
            hourFirst->setText(valueStr.substr(0, 1));
            hourSecond->setText(valueStr.substr(1, 1));
        }
    }

    auto ClockVertical::updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void
    {
        elem->setFont(fontName);
        elem->setMinimumHeightToFitText();
        elem->setMinimumWidthToFitText();

        elem->setText(elem->getText());
    }

    auto ClockVertical::setFont(const std::string &newFontName) noexcept -> void
    {
        fontName = newFontName;

        updateFont(hourFirst, fontName);
        updateFont(hourSecond, fontName);
        updateFont(minuteFirst, fontName);
        updateFont(minuteSecond, fontName);

        resizeItems();
    }

    auto ClockVertical::isPM() const noexcept -> bool
    {
        return timeFormat == utils::time::Locale::TimeFormat::FormatTime12H;
    }

    auto ClockVertical::getTimeFormat() const noexcept -> utils::time::Locale::TimeFormat
    {
        return timeFormat;
    }

    auto ClockVertical::setTime(std::time_t time) noexcept -> void
    {
        using namespace utils::time;
        const auto t     = std::localtime(&time);
        const auto hours = std::chrono::hours{t->tm_hour};

        switch (timeFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            const auto hours12H = date::make12(hours);
            setHour(hours12H.count());
        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            setHour(hours.count());
        } break;
        default:
            break;
        }

        setMinute(t->tm_min);
    }
} // namespace gui
