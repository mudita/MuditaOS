#pragma once

#include "Rect.hpp"
#include "Image.hpp"
#include "Label.hpp"

namespace gui
{
    class IntervalBox : public Rect
    {
        Label *topLabel         = nullptr;
        Label *bottomLabel      = nullptr;
        Image *leftSwitchArrow  = nullptr;
        Image *rightSwitchArrow = nullptr;

        void build();

      public:
        IntervalBox(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);

        bool onFocus(bool state) final;
        bool onInput(const InputEvent &inputEvent) final;
    };
} // namespace gui
