#include "TimerSetter.hpp"
#include <application-meditation/data/Style.hpp>

using namespace gui;

TimerSetter::TimerSetter(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h)
    : Rect(parent, x, y, w, h)
{
    build();
}

void TimerSetter::build()
{
    const Point boxCenter(getX() + (getWidth() / 2), getY() + (getHeight() / 2));

    namespace timerStyle = style::meditation::timer;
    Circle::ShapeParams params;
    params.setCenterPoint(boxCenter)
        .setRadius(timerStyle::Radius)
        .setBorderColor(timerStyle::BorderColor)
        .setFocusBorderColor(timerStyle::BorderColorOnFocused)
        .setPenWidth(timerStyle::PenWidth)
        .setFocusPenWidth(timerStyle::PenWidth);
    circle = new Circle(this, params);

    namespace timerStyle = style::meditation::timer;
    timeLabel            = new Label(this,
                          timerStyle::setterValueLabel::X,
                          timerStyle::setterValueLabel::Y,
                          timerStyle::setterValueLabel::Width,
                          timerStyle::setterValueLabel::Height);
    timeLabel->setEdges(RectangleEdge::None);
    timeLabel->setFont(style::window::font::supersizemelight);
    timeLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeLabel->setPenWidth(timerStyle::PenWidth);
    timeLabel->setText(std::to_string(state.getTime().count()));

    timeUnitLabel = new Label(this,
                              timerStyle::setterUnitLabel::X,
                              timerStyle::setterUnitLabel::Y,
                              timerStyle::setterUnitLabel::Width,
                              timerStyle::setterUnitLabel::Height);
    timeUnitLabel->setFont(style::window::font::verysmall);
    timeUnitLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeUnitLabel->setText("MINUTES");
    timeUnitLabel->setEdges(RectangleEdge::None);
}

bool TimerSetter::onFocus(bool isFocused)
{
    circle->setFocus(isFocused);
    if (isFocused) {
        timeLabel->setEdges(RectangleEdge::Bottom);
    }
    else {
        timeLabel->setEdges(RectangleEdge::None);
    }
    state.onFocus();
    return true;
}

bool TimerSetter::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
        bool handled = false;
        if (0 <= gui::toNumeric(inputEvent.keyCode) && gui::toNumeric(inputEvent.keyCode) <= 9) {
            state.putNumericValue(gui::toNumeric(inputEvent.keyCode));
            handled = true;
        }
        else if (inputEvent.is(KeyCode::KEY_LEFT)) {
            state.decrement();
            handled = true;
        }
        else if (inputEvent.is(KeyCode::KEY_RIGHT)) {
            state.increment();
            handled = true;
        }
        else {
            state.onFocus();
        }
        timeLabel->setText(std::to_string(state.getTime().count()));
        if (handled) {
            return true;
        }
    }
    return false;
}

std::chrono::seconds TimerSetter::getTime() noexcept
{
    state.checkBounds();
    return state.getTime();
}

void TimerSetter::State::checkBounds() noexcept
{
    timeInMinutes       = std::clamp(timeInMinutes, minimalValue, maximalValue);
    resetValueOnNumeric = true;
}

void TimerSetter::State::putNumericValue(int digit) noexcept
{
    if (resetValueOnNumeric) {
        timeInMinutes       = 0;
        resetValueOnNumeric = false;
    }
    timeInMinutes = 10 * timeInMinutes + digit;
    if (timeInMinutes >= minimalValue) {
        resetValueOnNumeric = true;
    }
}

void TimerSetter::State::increment() noexcept
{
    timeInMinutes       = (timeInMinutes / defaultIncrementValue + 1) * defaultIncrementValue;
    resetValueOnNumeric = true;
    checkBounds();
}

void TimerSetter::State::decrement() noexcept
{
    timeInMinutes       = ((timeInMinutes - 1) / defaultIncrementValue) * defaultIncrementValue;
    resetValueOnNumeric = true;
    checkBounds();
}
