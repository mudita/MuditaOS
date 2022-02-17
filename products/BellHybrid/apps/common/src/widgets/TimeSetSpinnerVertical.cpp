// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/TimeSetSpinnerVertical.hpp>

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <widgets/spinners/Spinners.hpp>

static constexpr auto hourMin         = 0;
static constexpr auto hourMax         = 23;
static constexpr auto hourDoubleDigit = 10;
static constexpr auto hourStep        = 1;
static constexpr auto minuteMin       = 0;
static constexpr auto minuteMax       = 59;
static constexpr auto minuteStep      = 1;

namespace gui
{

    TimeSetSpinnerVertical::TimeSetSpinnerVertical(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : VBox{parent, x, y, w, h}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hour =
            new UIntegerSpinnerFixed(UIntegerSpinnerFixed::Range{hourMin, hourMax, hourStep}, Boundaries::Continuous);
        updateFont(hour, fontName);

        hour->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        hour->setEdges(RectangleEdge::None);
        hour->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);
        hour->setCurrentValue(0);
        hour->setMargins(getHourMargins(fontName));

        addWidget(hour);

        minute = new UIntegerSpinnerFixed(UIntegerSpinnerFixed::Range{minuteMin, minuteMax, minuteStep},
                                          Boundaries::Continuous);
        updateFont(minute, fontName);
        minute->setPenFocusWidth(style::time_set_spinner_vertical::focus::size);

        minute->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        minute->setEdges(RectangleEdge::None);
        minute->setCurrentValue(0);
        addWidget(minute);

        resizeItems();

        setTimeFormat(timeFormat);
    }

    auto TimeSetSpinnerVertical::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        using namespace utils;

        switch (newFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            auto hours = std::chrono::hours(hour->getCurrentValue());
            hour->setRange(UIntegerSpinnerFixed::Range{
                time::Locale::min_hour_12H_mode, time::Locale::max_hour_12H_mode, hourStep});

            if (timeFormat != newFormat) {
                hour->setCurrentValue(date::make12(hours).count());
            }

        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            auto hours = std::chrono::hours(hour->getCurrentValue());
            hour->setRange(UIntegerSpinnerFixed::Range{
                time::Locale::min_hour_24H_mode, time::Locale::max_hour_24H_mode, hourStep});

            if (newFormat != timeFormat) {
                hour->setCurrentValue(date::make24(hours, isPM()).count());
            }
        } break;
        default:
            break;
        }

        timeFormat = newFormat;
    }

    auto TimeSetSpinnerVertical::setMinute(int value) noexcept -> void
    {
        minute->setCurrentValue(value);
    }

    auto TimeSetSpinnerVertical::getHour() const noexcept -> int
    {
        return hour->getCurrentValue();
    }

    auto TimeSetSpinnerVertical::getMinute() const noexcept -> int
    {
        return minute->getCurrentValue();
    }

    auto TimeSetSpinnerVertical::setHour(int value) noexcept -> void
    {
        hour->setCurrentValue(value);
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

        updateFont(hour, fontName);
        updateFont(minute, fontName);
        auto hourMargins = getHourMargins(fontName);
        hour->setMargins(getHourMargins(fontName));

        auto width = hour->widgetMinimumArea.w > minute->widgetMinimumArea.w ? hour->widgetMinimumArea.w
                                                                             : minute->widgetMinimumArea.w;

        auto verticalMargin = hourMargins.top + hourMargins.bottom;
        setMinimumSize(width, hour->widgetMinimumArea.h + minute->widgetMinimumArea.h + verticalMargin);
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
            hour->setCurrentValue(t->tm_hour);
            minute->setCurrentValue(t->tm_min);
        }
        else {
            const auto hours   = std::chrono::hours{t->tm_hour};
            const auto time12H = date::make12(hours);
            hour->setCurrentValue(time12H.count());
            minute->setCurrentValue(t->tm_min);
        }
    }

    auto TimeSetSpinnerVertical::getTime() const noexcept -> std::time_t
    {
        using namespace utils::time;
        const auto now     = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto newTime = std::localtime(&now);
        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            newTime->tm_hour = hour->getCurrentValue();
        }
        else {
            newTime->tm_hour = date::make24(std::chrono::hours{hour->getCurrentValue()}, isPM()).count();
        }
        newTime->tm_min = minute->getCurrentValue();

        return std::mktime(newTime);
    }

    void TimeSetSpinnerVertical::handleContentChanged()
    {
        hour->setMinimumWidthToFitText();
        minute->setMinimumWidthToFitText();

        auto width = hour->widgetMinimumArea.w > minute->widgetMinimumArea.w ? hour->widgetMinimumArea.w
                                                                             : minute->widgetMinimumArea.w;

        setMinimumSize(width, hour->widgetMinimumArea.h + minute->widgetMinimumArea.h + 10);
        setMaximumWidth(widgetMinimumArea.w);

        VBox::informContentChanged();
    }

    auto TimeSetSpinnerVertical::getHourMargins(const std::string &font) const noexcept -> Margins
    {
        return spaceMarginsMap.find(font)->second;
    }

} // namespace gui
