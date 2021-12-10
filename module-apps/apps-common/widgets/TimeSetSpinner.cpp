// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetSpinner.hpp"

#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/widgets/Label.hpp>

static constexpr auto hourMin         = 0;
static constexpr auto hourMax         = 23;
static constexpr auto hourDoubleDigit = 10;
static constexpr auto hourStep        = 1;
static constexpr auto minuteMin       = 0;
static constexpr auto minuteMax       = 59;
static constexpr auto minuteStep      = 1;

namespace gui
{
    TimeSetSpinner::TimeSetSpinner(Item *parent) : TimeSetSpinner(parent, 0, 0, 0, 0)
    {}

    TimeSetSpinner::TimeSetSpinner(Item *parent, Length x, Length y, Length w, Length h) : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hour = new UIntegerSpinner(UIntegerSpinner::Range{hourMin, hourMax, hourStep}, Boundaries::Continuous);
        updateFont(hour, noFocusFontName);

        hour->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        hour->setEdges(RectangleEdge::None);
        hour->setPenFocusWidth(style::time_set_spinner::focus::size);
        hour->setCurrentValue(0);

        addWidget(hour);

        colon = new ImageBox(this, 0, 0, 0, 0, new Image(getColonImage(noFocusFontName)));
        colon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        colon->setMargins(getColonMargins(noFocusFontName));
        colon->setEdges(RectangleEdge::None);
        colon->activeItem = false;

        minute = new UIntegerSpinnerFixed(UIntegerSpinnerFixed::Range{minuteMin, minuteMax, minuteStep},
                                          Boundaries::Continuous);
        updateFont(minute, noFocusFontName);
        minute->setPenFocusWidth(style::time_set_spinner::focus::size);

        minute->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        minute->setEdges(RectangleEdge::None);
        minute->setCurrentValue(0);
        addWidget(minute);

        resizeItems();

        focusChangedCallback = [&](Item &) {
            if (editMode != EditMode::Edit) {
                setFocusItem(nullptr);
                return false;
            }

            updateFocus(lastFocus != nullptr ? lastFocus : hour);
            return true;
        };
        if (editMode == EditMode::Edit) {
            updateFocus(hour);
        }
    }

    auto TimeSetSpinner::onInput(const InputEvent &inputEvent) -> bool
    {
        if (editMode != EditMode::Edit) {
            return false;
        }

        if (auto ret = this->focusItem->onInput(inputEvent)) {
            applySizeRestrictions();
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

    uint16_t TimeSetSpinner::getFontHeight(const std::string &fontName) const noexcept
    {
        const RawFont *font = FontManager::getInstance().getFont(fontName);
        return font->info.line_height;
    }

    auto TimeSetSpinner::handleEnterKey() -> bool
    {
        if (focusItem == hour) {
            updateFocus(minute);
            return true;
        }
        return false;
    }

    auto TimeSetSpinner::handleRightFunctionKey() -> bool
    {
        if (focusItem == minute) {
            updateFocus(hour);
            return true;
        }
        return false;
    }

    auto TimeSetSpinner::setTime(std::time_t time) noexcept -> void
    {
        const auto t = std::localtime(&time);
        setHour(t->tm_hour);
        setMinute(t->tm_min);
        applySizeRestrictions();
    }

    auto TimeSetSpinner::setTime(int hourValue, int minuteValue) noexcept -> void
    {
        setHour(hourValue);
        setMinute(minuteValue);
        applySizeRestrictions();
    }

    auto TimeSetSpinner::setHour(int value) noexcept -> void
    {
        hour->setCurrentValue(value);
    }

    auto TimeSetSpinner::setMinute(int value) noexcept -> void
    {
        minute->setCurrentValue(value);
    }

    auto TimeSetSpinner::setFont(const std::string &newFontName) noexcept -> void
    {
        setFont(newFontName, newFontName);
    }

    auto TimeSetSpinner::setFont(std::string newFocusFontName, std::string newNoFocusFontName) noexcept -> void
    {
        focusFontName   = std::move(newFocusFontName);
        noFocusFontName = std::move(newNoFocusFontName);

        updateFont(hour, noFocusFontName);
        updateFont(minute, noFocusFontName);

        colon->setImage(getColonImage(noFocusFontName));
        colon->setMargins(getColonMargins(noFocusFontName));
        colon->setMinimumSizeToFitImage();

        setMinimumSize(hour->widgetMinimumArea.w + colon->widgetMinimumArea.w +
                           colon->getMargins().getSumInAxis(Axis::X) + minute->widgetMinimumArea.w,
                       getFontHeight(noFocusFontName));
        resizeItems();
    }

    auto TimeSetSpinner::updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void
    {
        elem->setFont(fontName);
        elem->setMinimumHeightToFitText();
        elem->setMinimumWidthToFitText();

        elem->setText(elem->getText());
    }

    auto TimeSetSpinner::setEditMode(EditMode editMode) noexcept -> void
    {
        this->editMode = editMode;
        if (editMode == EditMode::Edit) {
            updateFocus(hour);
        }
        else {
            setFocusItem(nullptr);
        }
    }

    auto TimeSetSpinner::getHour() const noexcept -> int
    {
        return hour->getCurrentValue();
    }

    auto TimeSetSpinner::getMinute() const noexcept -> int
    {
        return minute->getCurrentValue();
    }

    void TimeSetSpinner::updateFocus(Item *newFocus)
    {
        if (focus) {
            setFocusItem(newFocus);
            lastFocus = newFocus;

            if (minute->focus) {
                updateFont(minute, focusFontName);
                updateFont(hour, noFocusFontName);
            }
            else if (hour->focus) {
                updateFont(hour, focusFontName);
                updateFont(minute, noFocusFontName);
            }
        }
        else {
            updateFont(hour, noFocusFontName);
            updateFont(minute, noFocusFontName);
            setFocusItem(nullptr);
        }

        applySizeRestrictions();
    }

    auto TimeSetSpinner::setHourRange(std::uint32_t min, std::uint32_t max) -> void
    {
        hour->setRange(UIntegerSpinner::Range{min, max, hourStep});
    }

    auto TimeSetSpinner::getColonImage(const std::string &colonFont) const noexcept -> std::string
    {
        return colonFontMap.find(colonFont)->second;
    }

    auto TimeSetSpinner::getColonMargins(const std::string &colonFont) const noexcept -> Margins
    {
        return colonMarginsMap.find(colonFont)->second;
    }

    auto TimeSetSpinner::applySizeRestrictions() -> void
    {
        hour->setMinimumWidthToFitText();
        minute->setMinimumWidthToFitText();

        setMinimumSize(hour->widgetMinimumArea.w + colon->widgetMinimumArea.w +
                           colon->getMargins().getSumInAxis(Axis::X) + minute->widgetMinimumArea.w,
                       getFontHeight(noFocusFontName));
        setMaximumWidth(widgetMinimumArea.w);

        HBox::informContentChanged();
    }
} /* namespace gui */
