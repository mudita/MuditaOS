// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "Clock.hpp"
#include "DrawCommand.hpp"

namespace
{
    constexpr auto minuteArrowLength = 202U;
    constexpr auto hourArrowLength   = 136U;
    constexpr auto minuteDegree      = 6.0f;
    constexpr auto hourDegree        = 30.0f;
    constexpr auto arrowBegin        = 90U;
    constexpr auto circleSize        = 15U;
    constexpr auto arrowHourWidth    = 16U;
    constexpr auto arrowMinuteWidth  = 10U;
} // namespace

namespace gui
{
    Clock::Clock(Item *parent) : Item{}
    {
        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
    }

    bool Clock::isPreviousEvent(const InputEvent &inputEvent)
    {
        return (inputEvent.is(KeyCode::KEY_DOWN) || inputEvent.is(KeyCode::KEY_LEFT));
    }

    bool Clock::isNextEvent(const InputEvent &inputEvent)
    {
        return (inputEvent.is(KeyCode::KEY_UP) || inputEvent.is(KeyCode::KEY_RIGHT));
    }

    bool Clock::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (isPreviousEvent(inputEvent)) {
                // setTime(storeHour, --storeMinute);
                return true;
            }
            else if (isNextEvent(inputEvent)) {
                // setTime(storeHour, ++storeMinute);
                return true;
            }
        }
        return false;
    }

    void Clock::setTime(int hour, int minute)
    {
        const auto fraction  = 0.5f * minute;
        const auto hourAngle = trigonometry::toRadians((hourDegree * hour) + fraction - arrowBegin);
        arrowEndHour.x       = static_cast<int>(center.x + hourArrowLength * cos(hourAngle));
        arrowEndHour.y       = static_cast<int>(center.y + hourArrowLength * sin(hourAngle));

        const auto minuteAngle = trigonometry::toRadians((minuteDegree * minute) - arrowBegin);
        arrowEndMinute.x       = static_cast<int>(center.x + minuteArrowLength * cos(minuteAngle));
        arrowEndMinute.y       = static_cast<int>(center.y + minuteArrowLength * sin(minuteAngle));

        storeHour   = hour;
        storeMinute = minute;
    }

    void Clock::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto arrowHour = std::make_unique<DrawLine>(center, arrowEndHour, ColorFullBlack, arrowHourWidth);
        commands.emplace_back(std::move(arrowHour));

        auto arrowMinute = std::make_unique<DrawLine>(center, arrowEndMinute, ColorFullBlack, arrowMinuteWidth);
        commands.emplace_back(std::move(arrowMinute));

        auto dot = std::make_unique<DrawCircle>(center, circleSize, 0, ColorFullBlack, true, ColorFullBlack);
        commands.emplace_back(std::move(dot));
    }
} // namespace gui
