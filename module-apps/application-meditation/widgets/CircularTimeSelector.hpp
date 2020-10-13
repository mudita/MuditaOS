#pragma once

#include "Circle.hpp"
#include "Rect.hpp"
#include "Text.hpp"
#include "Label.hpp"

namespace gui
{

    class CircularTimeSelector : public Rect
    {
        int timeInMinutes        = 0;
        Circle *circleNotFocused = nullptr;
        Circle *circleFocused    = nullptr;

        Label *timeLabel     = nullptr;
        Label *timeUnitLabel = nullptr;

        bool resetText = false;
        int value      = 0;

        void build();

      public:
        explicit CircularTimeSelector(
            Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);

        bool onFocus(bool state) final;
        bool onInput(const InputEvent &inputEvent) final;
        int getTimeInSeconds() const noexcept;
    };

} // namespace gui
