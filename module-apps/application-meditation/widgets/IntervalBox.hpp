// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "Image.hpp"
#include "Label.hpp"

#include <list>
#include <chrono>

namespace gui
{
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

            std::chrono::minutes getCurrent() const noexcept
            {
                return *current;
            }

            bool moveToNext(Direction dir) noexcept;
            [[nodiscard]] bool hasNext(Direction dir) const noexcept;

            [[nodiscard]] static std::string toPrintableInterval(std::chrono::minutes time);
        } chimeIntervals;

        Label *bottomLabel      = nullptr;
        Image *leftSwitchArrow  = nullptr;
        Image *rightSwitchArrow = nullptr;

        bool showLeftArrowOnFocus  = true;
        bool showRightArrowOnFocus = true;
        std::chrono::minutes intervalValue{0};

        void build();
        void updateIntervals(ChimeIntervalList::Direction dir);

      public:
        IntervalBox(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);

        bool onFocus(bool isFocused) final;
        bool onInput(const InputEvent &inputEvent) final;
        std::chrono::seconds getIntervalValue() const noexcept
        {
            return std::chrono::seconds{intervalValue};
        }
    };
} // namespace gui
