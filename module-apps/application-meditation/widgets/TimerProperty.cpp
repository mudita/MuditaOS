// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Style.hpp"
#include "TimerProperty.hpp"

#include <InputEvent.hpp>
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
    namespace timerStyle = style::meditation::timer;

    const Point boxCenter(getX() + (getWidth() / 2), getY() + (getHeight() / 2));
    Circle::ShapeParams params;
    params.setCenterPoint(boxCenter)
        .setRadius(timerStyle::Radius)
        .setBorderColor(timerStyle::BorderColor)
        .setFocusBorderColor(timerStyle::BorderColor)
        .setPenWidth(timerStyle::PenWidth)
        .setFocusPenWidth(timerStyle::FocusPenWidth);
    circle = new Circle(this, params);

    centerBody = new VBox(this, 0, 0, 2 * timerStyle::Radius, 2 * timerStyle::Radius);
    centerBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
    centerBody->setMinimumSize(2 * timerStyle::Radius, 2 * timerStyle::Radius);
    centerBody->setEdges(RectangleEdge::None);
    centerBody->activeItem = false;

    timeLabel = new Label(centerBody);
    timeLabel->setMargins(gui::Margins(0, timerStyle::setterValueLabel::TopMargin, 0, 0));
    timeLabel->setEdges(RectangleEdge::None);
    timeLabel->setMinimumSize(timerStyle::setterValueLabel::Width, timerStyle::setterValueLabel::Height);
    timeLabel->setFont(style::window::font::supersizemelight);
    timeLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeLabel->setPenWidth(timerStyle::PenWidth);
    timeLabel->setText(std::to_string(static_cast<int>(state.getTime().count())));

    divRect = new Rect(centerBody, 0, 0, timerStyle::setterValueLabel::BottomLineWidth, 1);
    divRect->setPenWidth(timerStyle::setterValueLabel::BottomLinePen);

    timeUnitLabel = new Label(centerBody);
    timeUnitLabel->setMinimumSize(timerStyle::setterUnitLabel::Width, timerStyle::setterUnitLabel::Height);
    timeUnitLabel->setMargins(gui::Margins(0, timerStyle::setterUnitLabel::TopMargin, 0, 0));
    timeUnitLabel->setFont(style::window::font::verysmall);
    timeUnitLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    timeUnitLabel->setEdges(RectangleEdge::None);
    timeUnitLabel->setText(utils::translate("app_meditation_minutes"));

    centerBody->resizeItems();
}

bool TimerProperty::onFocus(bool isFocused)
{
    circle->setFocus(isFocused);
    if (isFocused) {
        divRect->setEdges(RectangleEdge::Top);
    }
    else {
        divRect->setEdges(RectangleEdge::None);
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
        else if (inputEvent.is(KeyCode::KEY_PND)) {
            state.delNumericValue();
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
    if (timeInMinutes == 0 && digit == 0) {
        digit = minimalValue;
    }
    timeInMinutes = 10 * timeInMinutes + digit;
    if (timeInMinutes >= 10 * (counterMaxDigits - 1)) {
        resetValueOnNumeric = true;
    }
}
void TimerProperty::State::delNumericValue() noexcept
{
    timeInMinutes = timeInMinutes / 10;
    if (timeInMinutes < minimalValue) {
        timeInMinutes       = minimalValue;
        resetValueOnNumeric = true;
    }
    else {
        resetValueOnNumeric = false;
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
