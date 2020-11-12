// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextCursor.hpp"

namespace gui
{
    class TextLineCursor : public TextCursor
    {
      public:
        using TextCursor::TextCursor;

        uint32_t getScreenLine() const
        {
            return line;
        }

        auto moveCursor(NavigationDirection direction) -> Move
        {
            Move movement = TextCursor::moveCursor(direction);

            if (movement == Move::Down) {
                line++;
            }
            else if (movement == Move::Up) {
                line--;
            }

            return movement;
        }

      private:
        uint32_t line = 0;
    };
} // namespace gui