// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "Clock.hpp"
#include "DrawCommand.hpp"

namespace gui
{
    Clock::Clock(Item *parent) : Item{}
    {
        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
        updateArrows(Direction::Clockwise);
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
                updateArrows(Direction::Clockwise);
                return true;
            }
            else if (isNextEvent(inputEvent)) {
                updateArrows(Direction::Counterclockwise);
                return true;
            }
        }
        return false;
    }

    void Clock::updateArrows(Direction dir)
    {
        if (dir == Direction::Clockwise) {
            arrowDeg += stepDegree;
        }
        else {
            arrowDeg -= stepDegree;
        }

        float angle      = angleStep * arrowDeg;
        arrowEndMinute.x = static_cast<int>(center.x + 180.f * cos(angle));
        arrowEndMinute.y = static_cast<int>(center.y + 180.f * sin(angle));

        angle          = angleStep * arrowDeg / stepDegree;
        arrowEndHour.x = static_cast<int>(center.x + 90.f * cos(angle));
        arrowEndHour.y = static_cast<int>(center.y + 90.f * sin(angle));
    }

    void Clock::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto arrowHour = std::make_unique<DrawLine>(center, arrowEndHour, ColorFullBlack, arrowHourWidth);
        commands.emplace_back(std::move(arrowHour));

        auto arrowMinute = std::make_unique<DrawLine>(center, arrowEndMinute, ColorFullBlack, arrowMinuteWidth);
        commands.emplace_back(std::move(arrowMinute));

        auto dot = std::make_unique<DrawCircle>(center, 15, 0, ColorFullBlack, true, ColorFullBlack);
        commands.emplace_back(std::move(dot));
    }
} // namespace gui
