#include "IntervalBox.hpp"

#include "InputEvent.hpp"

using namespace gui;

IntervalBox::IntervalBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
    : Rect(parent, x, y, w, h)
{
    build();
}

void IntervalBox::build()
{
    topLabel = new Label(this, 0, 0, style::window::default_body_width, 40, "Interval Chime");
    topLabel->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Top));

    bottomLabel = new Label(this, 0, 40, style::window::default_body_width, 40, "Every x minutes");
    bottomLabel->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));

    leftSwitchArrow = new gui::Image(bottomLabel, 0, 10, 0, 0, "left_label_arrow");
    rightSwitchArrow =
        new gui::Image(bottomLabel, style::window::default_body_width - 15, 10, 0, 0, "right_label_arrow");

    leftSwitchArrow->setVisible(false);
    rightSwitchArrow->setVisible(false);
}
#include <iostream>
bool IntervalBox::onFocus(bool state)
{
    std::cout << "HERE I WORK IntervalBox::onFocus 1" << std::endl;
    if (state) {
        std::cout << "HERE I WORK IntervalBox::onFocus true" << std::endl;
        bottomLabel->setFont(style::window::font::smallbold);
    }
    else {
        std::cout << "HERE I WORK IntervalBox::onFocus false" << std::endl;
        bottomLabel->setFont(style::window::font::small);
    }
    leftSwitchArrow->setVisible(state);
    rightSwitchArrow->setVisible(state);
    return true;
}

bool IntervalBox::onInput(const InputEvent &inputEvent)
{
    std::cout << "HERE I WORK IntervalBox::onInput 1" << std::endl;
    if (inputEvent.isShortPress()) {
        if (inputEvent.is(KeyCode::KEY_LEFT)) {
            std::cout << "HERE I WORK IntervalBox::onInput KEY_LEFT" << std::endl;
            return true;
        }
        else if (inputEvent.is(KeyCode::KEY_RIGHT)) {
            std::cout << "HERE I WORK IntervalBox::onInput KEY_RIGHT" << std::endl;
            return true;
        }
    }
    std::cout << "HERE I WORK IntervalBox::onInput 2" << std::endl;
    return false;
}
