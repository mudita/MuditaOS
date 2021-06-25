// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetSpinner.hpp"
#include <module-gui/gui/widgets/Label.hpp>

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

        hour = new gui::Spinner(hourMin, hourMax, hourStep, gui::Boundaries::Continuous);
        hour->setMinimumSize(style::timeSetSpinner::timeText::w, style::timeSetSpinner::timeText::h);
        hour->setFont(style::window::font::supersizemelight);
        hour->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        hour->setCurrentValue(0);
        hour->setFocus(true);
        hbox->addWidget(hour);

        auto colon = new gui::Label(hbox,
                                    style::timeSetSpinner::colon::x,
                                    style::timeSetSpinner::colon::y,
                                    style::timeSetSpinner::colon::w,
                                    style::timeSetSpinner::colon::h);
        colon->setFont(style::window::font::supersizemelight);
        colon->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colon->setEdges(gui::RectangleEdge::None);
        colon->activeItem = false;
        colon->setText(":");

        minute = new gui::Spinner(minuteMin, minuteMax, minuteStep, gui::Boundaries::Continuous);
        minute->setMinimumSize(style::timeSetSpinner::timeText::w, style::timeSetSpinner::timeText::h);
        minute->setFont(style::window::font::supersizemelight);
        minute->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        minute->setCurrentValue(0);
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

    auto TimeSetSpinner::setHour(int value) const noexcept -> void
    {
        hour->setCurrentValue(value);
    }
    auto TimeSetSpinner::setMinute(int value) const noexcept -> void
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
