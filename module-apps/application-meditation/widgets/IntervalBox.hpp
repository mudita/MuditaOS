#pragma once

#include "Rect.hpp"
#include "Image.hpp"
#include "Label.hpp"

#include <list>

namespace gui
{
    class IntervalBox : public Rect
    {
        class ChimeIntervalList
        {
            std::list<int> intervals;
            std::list<int>::const_iterator current;

          public:
            enum Direction
            {
                Next,
                Previous
            };

            ChimeIntervalList();

            int get() const noexcept
            {
                return *current;
            }

            bool moveToNext(Direction) noexcept;
            bool hasNext(Direction) const noexcept;

            static std::string to_string(int);
        } chimeIntervals;

        Label *topLabel         = nullptr;
        Label *bottomLabel      = nullptr;
        Image *leftSwitchArrow  = nullptr;
        Image *rightSwitchArrow = nullptr;

        bool showLeftArrowOnFocus  = true;
        bool showRightArrowOnFocus = true;
        int intervalValue          = 0;

        void build();
        void updateIntervals(ChimeIntervalList::Direction);

      public:
        IntervalBox(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);

        bool onFocus(bool state) final;
        bool onInput(const InputEvent &inputEvent) final;
        int getIntervalValue() const noexcept
        {
            return intervalValue;
        };
    };
} // namespace gui
