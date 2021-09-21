// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Style.hpp"
#include "TimerProperty.hpp"

#include <i18n/i18n.hpp>
#include <Utils.hpp>

using namespace gui;

TimerProperty::TimerProperty(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h)
    : Rect(parent, x, y, w, h)
{
    build();
}

void TimerProperty::build()
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
    timeLabel->setText(std::to_string(static_cast<int>(state.getTime().count())));

    timeUnitLabel = new Label(this,
                              timerStyle::setterUnitLabel::X,
                              timerStyle::setterUnitLabel::Y,
                              timerStyle::setterUnitLabel::Width,
                              timerStyle::setterUnitLabel::Height);
    timeUnitLabel->setFont(style::window::font::verysmall);
    timeUnitLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeUnitLabel->setEdges(RectangleEdge::None);
    timeUnitLabel->setText(utils::translate("app_meditation_minutes"));
}

bool TimerProperty::onFocus(bool isFocused)
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

bool TimerProperty::onInput(const InputEvent &inputEvent)
{
    bool handled = false;
    if (inputEvent.isShortRelease()) {
        if (inputEvent.isDigit()) {
            state.putNumericValue(inputEvent.numericValue());
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
        setMeditationTime();
    }
    return handled;
}

void TimerProperty::setMeditationTime()
{
    const auto meditationTime = static_cast<int>(state.getTime().count());
    timeLabel->setText(utils::to_string(meditationTime));
    if (meditationTime == 1) {
        timeUnitLabel->setText(utils::translate("app_meditation_minute"));
    }
    else {
        timeUnitLabel->setText(utils::translate("app_meditation_minutes"));
    }
}

std::chrono::minutes TimerProperty::getTime() noexcept
{
    state.checkBounds();
    return state.getTime();
}

void TimerProperty::State::checkBounds() noexcept
{
    timeInMinutes       = std::clamp(timeInMinutes, minimalValue, maximalValue);
    resetValueOnNumeric = true;
}

void TimerProperty::State::putNumericValue(int digit) noexcept
{
    if (resetValueOnNumeric) {
        timeInMinutes       = 0;
        resetValueOnNumeric = false;
    }
    timeInMinutes = 10 * timeInMinutes + digit;
    if (timeInMinutes >= 10 * (counterMaxDigits - 1)) {
        resetValueOnNumeric = true;
    }
}

void TimerProperty::State::increment() noexcept
{
    auto it = std::upper_bound(std::begin(timeArr), std::end(timeArr), timeInMinutes);
    if (it == std::end(timeArr)) {
        it--;
    }
    timeInMinutes       = *it;
    resetValueOnNumeric = true;
}

void TimerProperty::State::decrement() noexcept
{
    auto it =
        std::upper_bound(std::rbegin(timeArr), std::rend(timeArr), timeInMinutes, [](int a, int b) { return a > b; });
    if (it == std::rend(timeArr)) {
        it--;
    }
    timeInMinutes       = *it;
    resetValueOnNumeric = true;
}
