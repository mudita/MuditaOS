// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetSpinner.hpp"

#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/widgets/Label.hpp>

static constexpr uint32_t hourMin    = 0;
static constexpr uint32_t hourMax    = 23;
static constexpr uint32_t hourStep   = 1;
static constexpr uint32_t minuteMin  = 0;
static constexpr uint32_t minuteMax  = 59;
static constexpr uint32_t minuteStep = 1;
static constexpr uint32_t noOfDigits = 2;

namespace gui
{
    TimeSetSpinner::TimeSetSpinner(Item *parent, Size size, Length x, Length y, Length w, Length h)
        : HBox(parent, x, y, w, h), colonIconData(getColonIconData(size))
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        hour = new Spinner(hourMin, hourMax, hourStep, Boundaries::Continuous);
        updateFont(hour, noFocusFontName);

        hour->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        hour->setFixedFieldWidth(noOfDigits);
        hour->setEdges(RectangleEdge::None);
        hour->setPenFocusWidth(style::time_set_spinner::focus::size);
        hour->setCurrentValue(0);

        addWidget(hour);

        colon = new ImageBox(this, 0, 0, colonIconData.w, colonIconData.h, new Image(colonIconData.iconName));
        colon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        colon->setEdges(RectangleEdge::None);
        colon->activeItem = false;

        minute = new Spinner(minuteMin, minuteMax, minuteStep, Boundaries::Continuous);
        updateFont(minute, noFocusFontName);
        minute->setPenFocusWidth(style::time_set_spinner::focus::size);

        minute->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        minute->setFixedFieldWidth(noOfDigits);
        minute->setEdges(RectangleEdge::None);
        minute->setCurrentValue(0);
        addWidget(minute);

        resizeItems();

        focusChangedCallback = [&](Item &) {
            if (editMode != EditMode::Edit) {
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

    uint32_t TimeSetSpinner::getWidestDigitWidth(const std::string &fontName) const noexcept
    {
        const RawFont *font = FontManager::getInstance().getFont(fontName);
        uint32_t maxWidth   = 0;
        for (uint32_t i = '0'; i <= '9'; i++) {
            const uint32_t width = font->getCharPixelWidth(i);
            if (width > maxWidth) {
                maxWidth = width;
            }
        }
        return maxWidth;
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

        setMinimumSize(hour->widgetMinimumArea.w + colonIconData.w + minute->widgetMinimumArea.w,
                       getFontHeight(noFocusFontName));
        resizeItems();
    }

    auto TimeSetSpinner::updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void
    {
        auto fontHeight = getFontHeight(fontName);
        auto doubleCharWidth =
            (getWidestDigitWidth(fontName) * noOfDigits) + (1 * noOfDigits); // two digits + minimal space beetween

        elem->setFont(fontName);
        elem->setMinimumSize(doubleCharWidth, fontHeight);
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

    auto TimeSetSpinner::setHourMax(std::uint32_t newMax) noexcept -> void
    {
        hour->setMaxValue(newMax);
    }

    auto TimeSetSpinner::setHourMin(std::uint32_t newMin) noexcept -> void
    {
        hour->setMinValue(newMin);
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
    }

    auto TimeSetSpinner::getColonIconData(Size size) const noexcept -> ColonIconData
    {
        using namespace style::time_set_spinner::colonIconSize;
        static const ColonIconData smallColonIconData  = {"alarm_colon_W_M", smallW, smallH};
        static const ColonIconData mediumColonIconData = {"alarm_colon_select_W_M", mediumW, mediumH};
        static const ColonIconData bigColonIconData    = {"alarm_colon_clock_W_M", bigW, bigH};
        switch (size) {
        case Size::SMALL:
            return smallColonIconData;
        case Size::MEDIUM:
            return mediumColonIconData;
        case Size::BIG:
            return bigColonIconData;
        }
        return {};
    }
} /* namespace gui */
