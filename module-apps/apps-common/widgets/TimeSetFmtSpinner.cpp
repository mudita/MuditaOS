// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetFmtSpinner.hpp"

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <widgets/spinners/Spinners.hpp>

namespace gui
{

    TimeSetFmtSpinner::TimeSetFmtSpinner(Item *parent) : HBox{parent}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        timeSetSpinner = new TimeSetSpinner(nullptr);
        timeSetSpinner->setFont(focusFontName, noFocusFontName);
        timeSetSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        auto textRange = {time::Locale::getAM(), time::Locale::getPM()};
        fmt            = new StringSpinner(textRange, Boundaries::Continuous);
        updateFmtFont(noFocusFontName);
        fmt->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        fmt->setMargins(getFmtMargins(noFocusFontName));
        fmt->setEdges(RectangleEdge::None);
        fmt->setVisible(false);
        fmt->setPenFocusWidth(style::time_set_spinner::focus::size);
        fmt->setEdges(RectangleEdge::Bottom);

        addWidget(timeSetSpinner);
        addWidget(fmt);

        setTimeFormat(timeFormat, false);

        resizeItems();

        focusChangedCallback = [&](Item &) {
            if (focus) {
                setTimeFormat(this->timeFormat);
                if (editMode == EditMode::Edit) {
                    setFocusItem(timeSetSpinner);
                }
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        fmt->focusChangedCallback = [&](Item &) {
            if (fmt->focus) {
                updateFmtFont(focusFontName);
            }
            else {
                updateFmtFont(noFocusFontName);
            }
            return true;
        };
    }

    auto TimeSetFmtSpinner::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        setTimeFormat(newFormat, true);
    }

    auto TimeSetFmtSpinner::setTimeFormat(utils::time::Locale::TimeFormat newFormat, bool informContentChanged) noexcept
        -> void
    {
        using namespace utils;

        switch (newFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            fmt->setVisible(true);
            auto hours = std::chrono::hours(timeSetSpinner->getHour());
            timeSetSpinner->setHourRange(time::Locale::min_hour_12H_mode, time::Locale::max_hour_12H_mode);

            if (timeFormat != newFormat) {
                timeSetSpinner->setHour(date::make12(hours).count());
                set12HPeriod(date::is_pm(hours) ? Period::PM : Period::AM);
            }

        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            fmt->setVisible(false);
            auto hours = std::chrono::hours(timeSetSpinner->getHour());
            timeSetSpinner->setHourRange(time::Locale::min_hour_24H_mode, time::Locale::max_hour_24H_mode);

            if (newFormat != timeFormat) {
                timeSetSpinner->setHour(date::make24(hours, isPM()).count());

                if (focusItem == fmt) {
                    setFocusItem(timeSetSpinner);
                }
            }
        } break;
        default:
            break;
        }

        timeFormat = newFormat;

        if (informContentChanged) {
            fmt->handleContentChanged();
        }
    }

    auto TimeSetFmtSpinner::setMinute(int value) noexcept -> void
    {
        timeSetSpinner->setMinute(value);
    }

    auto TimeSetFmtSpinner::setEditMode(EditMode newEditMode) noexcept -> void
    {
        editMode = newEditMode;
        if (editMode == EditMode::Edit) {
            setFocusItem(timeSetSpinner);
            timeSetSpinner->setEditMode(editMode);
        }
        else {
            setFocusItem(nullptr);
        }
    }

    auto TimeSetFmtSpinner::getHour() const noexcept -> int
    {
        return timeSetSpinner->getHour();
    }

    auto TimeSetFmtSpinner::getMinute() const noexcept -> int
    {
        return timeSetSpinner->getMinute();
    }

    auto TimeSetFmtSpinner::getFmtMargins(const std::string &fmtFont) const noexcept -> Margins
    {
        return fmtMarginsMap.find(fmtFont)->second;
    }

    auto TimeSetFmtSpinner::setHour(int value) noexcept -> void
    {
        timeSetSpinner->setHour(value);
    }

    auto TimeSetFmtSpinner::setFont(std::string newFontName) noexcept -> void
    {
        setFont(newFontName, newFontName);
    }

    auto TimeSetFmtSpinner::setFont(std::string newFocusFontName, std::string newNoFocusFontName) noexcept -> void
    {
        focusFontName   = std::move(newFocusFontName);
        noFocusFontName = std::move(newNoFocusFontName);

        timeSetSpinner->setFont(focusFontName, noFocusFontName);
        updateFmtFont(noFocusFontName);

        setMinimumSize(timeSetSpinner->widgetMinimumArea.w + fmt->widgetMinimumArea.w +
                           fmt->margins.getSumInAxis(Axis::X),
                       timeSetSpinner->widgetMinimumArea.h);
        resizeItems();
    }

    auto TimeSetFmtSpinner::updateFmtFont(const std::string &fontName) noexcept -> void
    {
        fmt->setFont(fontName);
        fmt->setMinimumWidthToFitText();
        fmt->setMinimumHeightToFitText();
        fmt->setMargins(getFmtMargins(fontName));
        fmt->setText(fmt->getText());
    }

    auto TimeSetFmtSpinner::onInput(const InputEvent &inputEvent) -> bool
    {
        // Ignore input event when not in edit mode
        if (editMode != EditMode::Edit) {
            return false;
        }

        if (auto ret = this->focusItem->onInput(inputEvent)) {
            return ret;
        }

        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_ENTER:
                return handleEnterKey();
            case KeyCode::KEY_RF:
                return handleRightFunctionKey();

            default:
                break;
            }
        }
        return false;
    }

    auto TimeSetFmtSpinner::handleEnterKey() -> bool
    {
        using namespace utils::time;
        if ((focusItem == timeSetSpinner) && (timeFormat == Locale::TimeFormat::FormatTime12H)) {
            setFocusItem(fmt);
            return true;
        }
        return false;
    }

    auto TimeSetFmtSpinner::handleRightFunctionKey() -> bool
    {
        if (focusItem == fmt) {
            setFocusItem(timeSetSpinner);
            return true;
        }

        return false;
    }

    auto TimeSetFmtSpinner::isPM() const noexcept -> bool
    {
        return fmt->value() == utils::time::Locale::getPM().c_str();
    }

    auto TimeSetFmtSpinner::getTimeFormat() const noexcept -> utils::time::Locale::TimeFormat
    {
        return timeFormat;
    }

    void TimeSetFmtSpinner::setVisible(bool value)
    {
        HBox::setVisible(value);
        // This is a workaround for fmt visibility not changed properly
        // It's probably caused by a bug in BoxLayout
        fmt->setVisible(value);
    }

    auto TimeSetFmtSpinner::setTime(std::time_t time) noexcept -> void
    {
        using namespace utils::time;
        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            timeSetSpinner->setTime(time);
        }
        else {
            const auto t       = std::localtime(&time);
            const auto hours   = std::chrono::hours{t->tm_hour};
            const auto time12H = date::make12(hours);
            const auto isPM    = date::is_pm(hours);
            fmt->set_value(isPM ? utils::time::Locale::getPM() : utils::time::Locale::getAM());
            timeSetSpinner->setTime(time12H.count(), t->tm_min);
        }
    }

    auto TimeSetFmtSpinner::setTimeFormatSpinnerVisibility(bool visibility) noexcept -> void
    {
        fmt->setVisible(visibility);
        resizeItems();
    }

    auto TimeSetFmtSpinner::getTime() const noexcept -> std::time_t
    {
        using namespace utils::time;
        const auto now     = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto newTime = std::localtime(&now);
        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            newTime->tm_hour = timeSetSpinner->getHour();
        }
        else {
            newTime->tm_hour = date::make24(std::chrono::hours{timeSetSpinner->getHour()}, isPM()).count();
        }
        newTime->tm_min = timeSetSpinner->getMinute();

        return std::mktime(newTime);
    }

    void TimeSetFmtSpinner::handleContentChanged()
    {
        auto widthToSet = timeSetSpinner->widgetMinimumArea.w;

        if (timeFormat == utils::time::Locale::TimeFormat::FormatTime12H && fmt->visible) {
            fmt->setMinimumWidthToFitText();
            fmt->setMargins(getFmtMargins(noFocusFontName));
            widthToSet =
                timeSetSpinner->widgetMinimumArea.w + fmt->widgetMinimumArea.w + fmt->margins.getSumInAxis(Axis::X);
        }

        setMinimumWidth(widthToSet);
        setMaximumWidth(widthToSet);

        HBox::handleContentChanged();
    }
    auto TimeSetFmtSpinner::set12HPeriod(const Period period) -> void
    {
        period == Period::AM ? fmt->set_value(utils::time::Locale::getAM())
                             : fmt->set_value(utils::time::Locale::getPM());
    }
    auto TimeSetFmtSpinner::getHour24Format() const noexcept -> std::chrono::hours
    {
        using namespace utils::time;
        if (timeFormat == Locale::TimeFormat::FormatTime24H) {
            return std::chrono::hours{timeSetSpinner->getHour()};
        }

        return date::make24(std::chrono::hours{timeSetSpinner->getHour()}, isPM());
    }

} // namespace gui
