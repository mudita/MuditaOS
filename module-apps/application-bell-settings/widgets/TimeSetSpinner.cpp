// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetSpinner.hpp"

#include <widgets/Label.hpp>
#include <FontManager.hpp>
#include <RawFont.hpp>

#include <memory>

namespace gui
{
    TimeSetSpinner::TimeSetSpinner(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description)
        : SideListItem{std::move(description)}
    {
        setSize(w, h);
        setEdges(RectangleEdge::None);

        auto hbox = new gui::HBox(this,
                                  style::window::bellDefaultVerticalPos,
                                  style::window::bellDefaultLeftMargin,
                                  this->getWidth(),
                                  this->getHeight());
        hbox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hbox->setEdges(gui::RectangleEdge::None);

        auto fontHeight = getFontHeight();
        auto doubleCharWidth =
            (getWidestDigitWidth() * noOfDigits) + (1 * noOfDigits); // two digits + minimal space beetween

        hour = new gui::Spinner(hourMin, hourMax, hourStep, gui::Boundaries::Continuous);
        hour->setMinimumSize(doubleCharWidth, fontHeight);
        hour->setFont(fontName);
        hour->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        hour->setCurrentValue(0);
        hour->setFixedFieldWidth(noOfDigits);
        hour->setFocus(true);
        hbox->addWidget(hour);

        auto colon = new gui::Label(hbox,
                                    style::timeSetSpinner::colon::x,
                                    style::timeSetSpinner::colon::y,
                                    style::timeSetSpinner::colon::w,
                                    style::timeSetSpinner::colon::h);
        colon->setFont(fontName);
        colon->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colon->setEdges(gui::RectangleEdge::None);
        colon->activeItem = false;
        colon->setText(":");

        minute = new gui::Spinner(minuteMin, minuteMax, minuteStep, gui::Boundaries::Continuous);
        minute->setMinimumSize(doubleCharWidth, fontHeight);
        minute->setFont(fontName);
        minute->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        minute->setCurrentValue(0);
        minute->setFixedFieldWidth(noOfDigits);
        hbox->addWidget(minute);

        hbox->resizeItems();

        setFocusItem(hour);
    }

    auto TimeSetSpinner::onInput(const gui::InputEvent &inputEvent) -> bool
    {
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
        return this->focusItem->onInput(inputEvent);
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
        for (uint32_t i = '0'; i < '9'; i++) {
            uint32_t width = font->getCharPixelWidth(i);
            if (width > maxWidth) {
                maxWidth = width;
            }
        }
        return maxWidth;
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

    auto TimeSetSpinner::getHour() const noexcept -> int
    {
        return hour->getCurrentValue();
    }
    auto TimeSetSpinner::getMinute() const noexcept -> int
    {
        return minute->getCurrentValue();
    }
} /* namespace gui */
