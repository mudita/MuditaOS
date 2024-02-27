// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include <cstdint>

#include <Math.hpp>
#include "InputEvent.hpp"

#include "Common.hpp"
#include "Item.hpp"
#include "Style.hpp"

namespace gui
{
    namespace
    {
        static constexpr auto displayX         = 600U;
        static constexpr auto displayY         = 480U;
        static constexpr auto middleX          = displayX / 2;
        static constexpr auto middleY          = displayY / 2;
        static constexpr gui::Point center     = {middleX, middleY};
        static constexpr auto arrowHourWidth   = 16U;
        static constexpr auto arrowMinuteWidth = 10U;
        static constexpr auto stepDegree       = 6U;
        static constexpr auto points           = 360U;
        static constexpr auto angleStep        = (2.0f * M_PI) / points;
    } // namespace
    class Clock : public Item
    {
      public:
        Clock(Item *parent);

        bool isPreviousEvent(const InputEvent &inputEvent);
        bool isNextEvent(const InputEvent &inputEvent);
        bool onInput(const InputEvent &inputEvent);

        void buildDrawListImplementation(std::list<Command> &commands) override;

      private:
        enum class Direction
        {
            Clockwise,
            Counterclockwise
        };

        Point arrowEndMinute{center};
        Point arrowEndHour{center};
        std::int32_t arrowDeg{0};

        void updateArrows(Direction dir);
    };
} // namespace gui
