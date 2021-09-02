// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextFixedSize.hpp"

namespace gui
{
    class Spinner : public TextFixedSize
    {
      public:
        Spinner(int minValue, int maxValue, int step, Boundaries boundaries);

        void setCurrentValue(int newCurrent);
        void setFixedFieldWidth(unsigned char newFixedFieldWidth);

        void setMinValue(int newMinValue);
        void setMaxValue(int newMaxValue);
        void setFocusEdges(RectangleEdge edges);

        [[nodiscard]] int getCurrentValue() const noexcept;
        [[nodiscard]] unsigned char getFixedFieldWidth() const noexcept;

        void stepUp();
        void stepDown();

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        bool onFocus(bool state) override;

      private:
        int minValue;
        int maxValue;
        int step;
        int currentValue;
        Boundaries boundaries         = Boundaries::Continuous;
        unsigned char fixedFieldWidth = 0; ///< defines number of chars always displayed,
        ///< if current < fixedFieldWidth Label will be filled with 0
        ///< value of 0 means no fixed width.
        RectangleEdge focusEdges = RectangleEdge::Bottom;
        void updateSpinner();
    };
} // namespace gui
