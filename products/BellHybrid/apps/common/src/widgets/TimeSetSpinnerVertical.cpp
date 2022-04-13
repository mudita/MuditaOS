// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/TimeSetSpinnerVertical.hpp>
#include "widgets/LayoutVertical.hpp"

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <widgets/spinners/Spinners.hpp>

namespace gui
{

    TimeSetSpinnerVertical::TimeSetSpinnerVertical(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : VBox{parent, x, y, w, h}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hBoxHours = new gui::HBox(this, 0, 0, style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);
        hBoxHours->setEdges(gui::RectangleEdge::None);
        
        hour_first = new gui::TextFixedSize(hBoxHours, 0, 0, 0, 0);
        updateFont(hour_first, fontName);

        hour_first->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        hour_first->setEdges(RectangleEdge::None);
        hour_first->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);
        hour_first->setText("0");
        hour_first->setMargins(getHourMargins(fontName));

        hour_second = new gui::TextFixedSize(hBoxHours, 0, 0, 0, 0);
        updateFont(hour_second, fontName);

        hour_second->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        hour_second->setEdges(RectangleEdge::None);
        hour_second->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);
        hour_second->setText("0");
        hour_second->setMargins(getHourMargins(fontName));

        hBoxHours->addWidget(hour_first);
        hBoxHours->addWidget(hour_second);

        hBoxMinutes = new gui::HBox(this, 0, 0, style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);
        hBoxMinutes->setEdges(gui::RectangleEdge::None);

        minute_first = new gui::TextFixedSize(hBoxMinutes, 0, 0, 0, 0);
        updateFont(minute_first, fontName);

        minute_first->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        minute_first->setEdges(RectangleEdge::None);
        minute_first->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);
        minute_first->setText("0");

        minute_second = new gui::TextFixedSize(hBoxMinutes, 0, 0, 0, 0);
        updateFont(minute_second, fontName);

        minute_second->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        minute_second->setEdges(RectangleEdge::None);
        minute_second->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);
        minute_second->setText("0");

        hBoxMinutes->addWidget(minute_first);
        hBoxMinutes->addWidget(minute_second);

        resizeItems();

        setTimeFormat(timeFormat);
    }

    auto TimeSetSpinnerVertical::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        using namespace utils;

        switch (newFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            auto hourFirst = hour_first->getText();
            auto hourSecond = hour_second->getText();
            auto hoursStr = hourFirst + hourSecond;
            auto hours = std::chrono::hours(std::stoi(hoursStr.c_str()));

            if (timeFormat != newFormat) {
                const auto time12H = date::make12(hours);
                auto stringHour = std::to_string(time12H.count());
                hour_first->setText(stringHour.substr(0,1));
                hour_second->setText(stringHour.substr(1,1));
            }

        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            auto hourFirst = hour_first->getText();
            auto hourSecond = hour_second->getText();
            auto hoursStr = hourFirst + hourSecond;
            auto hours = std::chrono::hours(std::stoi(hoursStr.c_str()));

            if (newFormat != timeFormat) {
                auto time24H = date::make24(hours, isPM());
                auto stringHour = std::to_string(time24H.count());
                hour_first->setText(stringHour.substr(0,1));
                hour_second->setText(stringHour.substr(1,1));
            }
        } break;
        default:
            break;
        }

        timeFormat = newFormat;
    }

    // auto TimeSetSpinnerVertical::setMinute(int value) noexcept -> void
    // {
    //     minute->set_value(value);
    // }

    // auto TimeSetSpinnerVertical::getHour() const noexcept -> int
    // {
    //     return hour->value();
    // }

    // auto TimeSetSpinnerVertical::getMinute() const noexcept -> int
    // {
    //     return minute->value();
    // }

    // auto TimeSetSpinnerVertical::setHour(int value) noexcept -> void
    // {
    //     hour->set_value(value);
    // }

    auto TimeSetSpinnerVertical::updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void
    {
        elem->setFont(fontName);
        elem->setMinimumHeightToFitText();
        elem->setMinimumWidthToFitText();

        elem->setText(elem->getText());
    }

    auto TimeSetSpinnerVertical::setFont(const std::string &newFontName) noexcept -> void
    {
        fontName = newFontName;

        updateFont(hour_first, fontName);
        updateFont(hour_second, fontName);
        updateFont(minute_first, fontName);
        updateFont(minute_second, fontName);
        auto hourMargins = getHourMargins(fontName);
        hour_first->setMargins(getHourMargins(fontName));
        hour_second->setMargins(getHourMargins(fontName));

        auto hourArea = hour_first->widgetMinimumArea.w + hour_second->widgetMinimumArea.w;
        auto minuteArea = minute_first->widgetMinimumArea.w + minute_second->widgetMinimumArea.w;

        auto width = hourArea > minuteArea ? hourArea : minuteArea;

        auto verticalMargin = hourMargins.top + hourMargins.bottom;
        setMinimumSize(width, hourArea + minuteArea + verticalMargin);
        resizeItems();
    }

    auto TimeSetSpinnerVertical::isPM() const noexcept -> bool
    {
        return timeFormat == utils::time::Locale::TimeFormat::FormatTime12H;
    }

    auto TimeSetSpinnerVertical::getTimeFormat() const noexcept -> utils::time::Locale::TimeFormat
    {
        return timeFormat;
    }

    auto TimeSetSpinnerVertical::setTime(std::time_t time) noexcept -> void
    {
        using namespace utils::time;
        const auto t = std::localtime(&time);

        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            const auto hours   = std::chrono::hours{t->tm_hour};
            const auto time24H = date::make24(hours, isPM());
            auto stringHour = std::to_string(time24H.count());
            hour_first->setText(stringHour.substr(0,1));
            hour_second->setText(stringHour.substr(1,1));

            auto stringMin = std::to_string(t->tm_min);
            //minute_first->setText(stringMin.substr(0,1));
            //minute_second->setText(stringMin.substr(1,1));
            minute_first->setText("0");
            minute_second->setText("0");
        }
        else {
            const auto hours   = std::chrono::hours{t->tm_hour};
            const auto time12H = date::make12(hours);
            auto stringHour = std::to_string(time12H.count());
            hour_first->setText(stringHour.substr(0,1));
            hour_second->setText(stringHour.substr(1,1));

            auto stringMin = std::to_string(t->tm_min);
            //minute_first->setText(stringMin.substr(0,1));
            //minute_second->setText(stringMin.substr(1,1));
            minute_first->setText("0");
            minute_second->setText("0");
        }

        handleContentChanged();
    }

    // auto TimeSetSpinnerVertical::getTime() const noexcept -> std::time_t
    // {
    //     using namespace utils::time;
    //     const auto now     = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    //     const auto newTime = std::localtime(&now);
    //     if (timeFormat == Locale::TimeFormat::FormatTime24H) {
    //         newTime->tm_hour = hour->value();
    //     }
    //     else {
    //         newTime->tm_hour = date::make24(std::chrono::hours{hour->value()}, isPM()).count();
    //     }
    //     newTime->tm_min = minute->value();

    //     return std::mktime(newTime);
    // }

    void TimeSetSpinnerVertical::handleContentChanged()
    {
        hour_first->setMinimumWidthToFitText();
        hour_second->setMinimumWidthToFitText();
        minute_first->setMinimumWidthToFitText();
        minute_second->setMinimumWidthToFitText();

        auto hourArea = hour_first->widgetMinimumArea.w + hour_second->widgetMinimumArea.w;
        auto minuteArea = minute_first->widgetMinimumArea.w + minute_second->widgetMinimumArea.w;

        auto width = hourArea > minuteArea ? hourArea : minuteArea;

        setMinimumSize(width, hourArea + minuteArea + 10);
        setMaximumWidth(widgetMinimumArea.w);

        VBox::informContentChanged();
    }

    auto TimeSetSpinnerVertical::getHourMargins(const std::string &font) const noexcept -> Margins
    {
        return spaceMarginsMap.find(font)->second;
    }

} // namespace gui
