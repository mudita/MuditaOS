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

        RawFont *getFont()
        {
            auto format = (*this)->getFormat();
            return format == nullptr ? default_font : format->getFont();
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