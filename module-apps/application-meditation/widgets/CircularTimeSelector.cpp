#include "CircularTimeSelector.hpp"
#include <application-meditation/data/Style.hpp>

using namespace gui;
namespace timerStyle = style::meditation::timer;
#include <iostream>
CircularTimeSelector::CircularTimeSelector(
    Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
    : Rect(parent, x, y, w, h)
{
    build();
}

void CircularTimeSelector::build()
{
    const Point boxCenter(getX() + (getWidth() / 2), getY() + (getHeight() / 2));
    Circle::ShapeParams params;
    params.setCenterPoint(boxCenter)
        .setRadius(timerStyle::Radius)
        .setBorderColor(timerStyle::BorderColor)
        .setPenWidth(timerStyle::PenWidth)
        .setFocusPenWidth(timerStyle::PenWidth);
    circleNotFocused = new Circle(this, params);

    params.setBorderColor(timerStyle::BorderColorOnFocused);
    circleFocused = new Circle(this, params);
    circleFocused->setVisible(false);

    setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

    timeLabel = new Label(this, 100, 110, getWidth() - 200, getHeight() - 220);
    timeLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    timeLabel->setFont(style::window::font::supersizemelight);
    timeLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeLabel->setPenWidth(3);
    timeLabel->setText(std::to_string(value));

    timeUnitLabel = new Label(this, 0, getHeight() - 220 + 110, getWidth(), 40);
    timeUnitLabel->setFont(style::window::font::medium);
    timeUnitLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeUnitLabel->setText("minutes");
    timeUnitLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

    //    text->setVisible(true);
}

bool CircularTimeSelector::onFocus(bool state)
{
    resetText = state;
    circleNotFocused->setVisible(!state);
    circleFocused->setVisible(state);
    if (state) {
        timeLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    }
    else {
        timeLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }
    return true;
}

bool CircularTimeSelector::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
        if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            if (resetText) {
                value     = 0;
                resetText = false;
            }
            value = 10 * value + gui::toNumeric(inputEvent.keyCode);
            timeLabel->setText(std::to_string(value));
            return true;
        }
    }
    return false;
}

int CircularTimeSelector::getTimeInSeconds() const noexcept
{
    return 60 * timeInMinutes;
}
