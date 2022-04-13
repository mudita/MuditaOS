// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/TimeSetSpinnerVertical.hpp>
#include "widgets/LayoutVertical.hpp"

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <widgets/spinners/Spinners.hpp>

static constexpr auto doubleDigit = 10;

namespace gui
{

    TimeSetSpinnerVertical::TimeSetSpinnerVertical(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : VBox{parent, x, y, w, h}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hBoxHours = new gui::HBox(
            this, 0, 0, style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);
        hBoxHours->setEdges(gui::RectangleEdge::None);
        hBoxHours->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));

        hourFirst = new gui::TextFixedSize(hBoxHours, 0, 0, 0, 0);
        updateFont(hourFirst, fontName);

        hourFirst->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        hourFirst->setEdges(RectangleEdge::None);
        hourFirst->setText("0");
        hourFirst->setMargins(getHourMargins(fontName));

        hourSecond = new gui::TextFixedSize(hBoxHours, 0, 0, 0, 0);
        updateFont(hourSecond, fontName);

        hourSecond->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        hourSecond->setEdges(RectangleEdge::None);
        hourSecond->setText("0");
        hourSecond->setMargins(getHourMargins(fontName));

        hBoxHours->addWidget(hourFirst);
        hBoxHours->addWidget(hourSecond);

        hBoxMinutes = new gui::HBox(
            this, 0, 0, style::homescreen_vertical::center_box_w, style::homescreen_vertical::center_box_h);
        hBoxMinutes->setEdges(gui::RectangleEdge::None);
        hBoxMinutes->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        hBoxMinutes->setMargins(gui::Margins(0, 0, 0, style::homescreen_vertical::bottomMargin));

        minuteFirst = new gui::TextFixedSize(hBoxMinutes, 0, 0, 0, 0);
        updateFont(minuteFirst, fontName);

        minuteFirst->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Top));
        minuteFirst->setEdges(RectangleEdge::None);
        minuteFirst->setText("0");
        minuteFirst->drawUnderline(false);

        minuteSecond = new gui::TextFixedSize(hBoxMinutes, 0, 0, 0, 0);
        updateFont(minuteSecond, fontName);

        minuteSecond->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Top));
        minuteSecond->setEdges(RectangleEdge::None);
        minuteSecond->setText("0");
        minuteSecond->drawUnderline(false);

        hBoxMinutes->addWidget(minuteFirst);
        hBoxMinutes->addWidget(minuteSecond);

        resizeItems();

        setTimeFormat(timeFormat);
    }

    auto TimeSetSpinnerVertical::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        using namespace utils;

        const auto hourFirstStr  = hourFirst->getText();
        const auto hourSecondStr = hourSecond->getText();
        const auto hoursStr      = hourFirstStr + hourSecondStr;
        const auto hoursInt      = std::stoi(hoursStr.c_str());
        const auto hours         = std::chrono::hours(hoursInt);

        switch (newFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            if (timeFormat != newFormat) {
                const auto time12H    = date::make12(hours);
                const auto stringHour = std::to_string(time12H.count());
                if (hoursInt < doubleDigit) {
                    hourFirst->setText("0");
                    hourSecond->setText(stringHour.substr(0, 1));
                }
                else {
                    hourFirst->setText(stringHour.substr(0, 1));
                    hourSecond->setText(stringHour.substr(1, 1));
                }
            }

        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            if (newFormat != timeFormat) {
                const auto time24H    = date::make24(hours, isPM());
                const auto stringHour = std::to_string(time24H.count());
                if (hoursInt < doubleDigit) {
                    hourFirst->setText("0");
                    hourSecond->setText(stringHour.substr(0, 1));
                }
                else {
                    hourFirst->setText(stringHour.substr(0, 1));
                    hourSecond->setText(stringHour.substr(1, 1));
                }
            }
        } break;
        default:
            break;
        }

        timeFormat = newFormat;
    }

    auto TimeSetSpinnerVertical::setMinute(int value) noexcept -> void
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

    auto TimeSetSpinnerVertical::getHour() const noexcept -> int
    {
        auto hourFirstStr  = hourFirst->getText();
        auto hourSecondStr = hourSecond->getText();
        auto hoursStr      = hourFirstStr + hourSecondStr;
        return std::stoi(hoursStr.c_str());
    }

    auto TimeSetSpinnerVertical::getMinute() const noexcept -> int
    {
        auto minuteFirstStr  = minuteFirst->getText();
        auto minuteSecondStr = minuteSecond->getText();
        auto minutesStr      = minuteFirstStr + minuteSecondStr;
        return std::stoi(minutesStr.c_str());
    }

    auto TimeSetSpinnerVertical::setHour(int value) noexcept -> void
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

        updateFont(hourFirst, fontName);
        updateFont(hourSecond, fontName);
        updateFont(minuteFirst, fontName);
        updateFont(minuteSecond, fontName);
        auto hourMargins = getHourMargins(fontName);
        hourFirst->setMargins(getHourMargins(fontName));
        hourSecond->setMargins(getHourMargins(fontName));

        auto hourArea   = hourFirst->widgetMinimumArea.w + hourSecond->widgetMinimumArea.w;
        auto minuteArea = minuteFirst->widgetMinimumArea.w + minuteSecond->widgetMinimumArea.w;

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
        const auto hours = std::chrono::hours{t->tm_hour};

        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            const auto time24H = date::make24(hours, isPM());
            auto stringHour    = std::to_string(time24H.count());

            if (t->tm_hour < doubleDigit) {
                hourFirst->setText("0");
                hourSecond->setText(stringHour.substr(0, 1));
            }
            else {
                hourFirst->setText(stringHour.substr(0, 1));
                hourSecond->setText(stringHour.substr(1, 1));
            }
        }
        else {
            const auto time12H = date::make12(hours);
            auto stringHour    = std::to_string(time12H.count());
            auto hourInt       = std::stoi(stringHour.c_str());

            if (hourInt < doubleDigit) {
                hourFirst->setText("0");
                hourSecond->setText(stringHour.substr(0, 1));
            }
            else {
                hourFirst->setText(stringHour.substr(0, 1));
                hourSecond->setText(stringHour.substr(1, 1));
            }
        }

        setMinute(t->tm_min);
        handleContentChanged();
    }

    void TimeSetSpinnerVertical::handleContentChanged()
    {
        hourFirst->setMinimumWidthToFitText();
        hourSecond->setMinimumWidthToFitText();
        minuteFirst->setMinimumWidthToFitText();
        minuteSecond->setMinimumWidthToFitText();

        auto hourArea   = hourFirst->widgetMinimumArea.w + hourSecond->widgetMinimumArea.w;
        auto minuteArea = minuteFirst->widgetMinimumArea.w + minuteSecond->widgetMinimumArea.w;

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
