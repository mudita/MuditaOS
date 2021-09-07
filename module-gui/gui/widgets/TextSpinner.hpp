// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextFixedSize.hpp"

#include <string>
#include <vector>

namespace gui
{
    class TextSpinner : public TextFixedSize
    {
      public:
        using TextRange = std::vector<UTF8>;
        using Position  = std::uint32_t;
        using Range     = std::pair<Position, Position>;

        TextSpinner() = delete;
        TextSpinner(const TextRange &range, Boundaries boundaries);

        /// Sets range of strings to spin over
        void setTextRange(const TextRange &range);
        /// Sets current position. Must be within valid @ref Range that can be checked by @ref getValidRange
        void setCurrentPosition(Position pos);
        void setFocusEdges(RectangleEdge edges);

        [[nodiscard]] UTF8 getCurrentText() const noexcept;
        [[nodiscard]] Range getValidRange() const noexcept;

        void stepUp();
        void stepDown();

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        bool onFocus(bool state) override;

      private:
        TextRange textRange;
        Position currentPosition = 0;
        Boundaries boundaries    = Boundaries::Continuous;
        RectangleEdge focusEdges = RectangleEdge::Bottom;

        void update();
        Position getRangeUpLimit() const;
        Position getRangeDownLimit() const;
    };
} // namespace gui
