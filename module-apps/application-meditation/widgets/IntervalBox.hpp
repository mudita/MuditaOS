// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>

#include <list>
#include <chrono>

namespace gui
{
    class TimerProperty;

    class IntervalBox : public BoxLayout
    {
        class ChimeIntervalList
        {
            std::vector<std::chrono::minutes> intervals;
            std::vector<std::chrono::minutes>::const_iterator current;

          public:
            enum class Direction
            {
                Next,
                Previous
            };

            ChimeIntervalList();

            [[nodiscard]] std::chrono::minutes getCurrent() const noexcept
            {
                return *current;
            }

            bool moveToNext(Direction dir, std::chrono::minutes meditationTime) noexcept;
            [[nodiscard]] bool hasNext(Direction dir, std::chrono::minutes meditationTime) const noexcept;

            [[nodiscard]] static std::string toPrintableInterval(std::chrono::minutes time);
        } chimeIntervals;

        TimerProperty *timerSetter = nullptr;
        Label *bottomLabel      = nullptr;
        Image *leftSwitchArrow  = nullptr;
        Image *rightSwitchArrow = nullptr;

        std::chrono::minutes intervalValue{0};

        void build();
        void updateIntervals(ChimeIntervalList::Direction dir);
        void rescaleIntervals();

      public:
        IntervalBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TimerProperty *timerSetter);

        bool onFocus(bool isFocused) final;
        bool onInput(const InputEvent &inputEvent) final;
        [[nodiscard]] std::chrono::seconds getIntervalValue() const noexcept
        {
            return std::chrono::seconds{intervalValue};
        }
    };
} // namespace gui
