// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetSpinner.hpp"

#include <FontManager.hpp>
#include <RawFont.hpp>
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
    TimeSetSpinner::TimeSetSpinner(Item *parent, Length x, Length y, Length w, Length h) : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        auto fontHeight = getFontHeight();
        auto doubleCharWidth =
            (getWidestDigitWidth() * noOfDigits) + (1 * noOfDigits); // two digits + minimal space beetween

        hour = new Spinner(hourMin, hourMax, hourStep, Boundaries::Continuous);
        hour->setMinimumSize(doubleCharWidth, fontHeight);
        hour->setFont(fontName);
        hour->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        hour->setFixedFieldWidth(noOfDigits);
        hour->setEdges(RectangleEdge::None);
        hour->setCurrentValue(0);

        addWidget(hour);

        colon = new Label(this);
        colon->setFont(fontName);
        colon->setMinimumSize(getColonDigitWidth(), fontHeight);
        colon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        colon->setEdges(RectangleEdge::None);
        colon->activeItem = false;
        colon->setText(":");

        minute = new Spinner(minuteMin, minuteMax, minuteStep, Boundaries::Continuous);
        minute->setMinimumSize(doubleCharWidth, fontHeight);
        minute->setFont(fontName);
        minute->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        minute->setFixedFieldWidth(noOfDigits);
        minute->setEdges(RectangleEdge::None);
        minute->setCurrentValue(0);
        addWidget(minute);

        resizeItems();

        if (editMode == EditMode::Edit) {
            setFocusItem(hour);
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

    uint16_t TimeSetSpinner::getFontHeight() const noexcept
    {
        const RawFont *font = FontManager::getInstance().getFont(fontName);
        return font->info.line_height;
    }

    uint32_t TimeSetSpinner::getWidestDigitWidth() const noexcept
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

    uint32_t TimeSetSpinner::getColonDigitWidth() const noexcept
    {
        const RawFont *font = FontManager::getInstance().getFont(fontName);
        return font->getCharPixelWidth(':');
    }

    auto TimeSetSpinner::handleEnterKey() -> bool
    {
        if (focusItem == hour) {
            setFocusItem(minute);
            return true;
        }
        return false;
    }

    auto TimeSetSpinner::handleRightFunctionKey() -> bool
    {
        if (focusItem == minute) {
            setFocusItem(hour);
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

    auto TimeSetSpinner::setFont(std::string newFontName) noexcept -> void
    {
        fontName        = std::move(newFontName);
        auto fontHeight = getFontHeight();
        auto doubleCharWidth =
            (getWidestDigitWidth() * noOfDigits) + (1 * noOfDigits); // two digits + minimal space beetween

        hour->setFont(fontName);
        hour->setMinimumSize(doubleCharWidth, fontHeight);
        hour->setText(hour->getText());
        colon->setFont(fontName);
        colon->setMinimumSize(getColonDigitWidth(), fontHeight);
        colon->setText(":");
        minute->setFont(fontName);
        minute->setMinimumSize(doubleCharWidth, fontHeight);
        minute->setText(minute->getText());
        setMinimumSize(doubleCharWidth * 2 + getColonDigitWidth(), fontHeight);
        resizeItems();
    }

    auto TimeSetSpinner::setEditMode(EditMode editMode) noexcept -> void
    {
        this->editMode = editMode;
        if (editMode == EditMode::Edit) {
            setFocusItem(hour);
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
} /* namespace gui */
