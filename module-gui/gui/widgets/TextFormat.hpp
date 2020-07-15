#pragma once

#include <Color.hpp>
#include <functional>

namespace gui
{
    class RawFont;

    class TextFormat
    {
      private:
        RawFont *font = nullptr;
        Color color = ColorFullBlack;

        static constexpr auto setter = [](auto &local, auto &next) {
            if (local != next) {
                local = next;
            }
        };

      public:
        TextFormat(RawFont *font, Color color = {}) : font(font), color(color){};
        TextFormat(const TextFormat &) = default;

        [[nodiscard]] auto getFont() const
        {
            return font;
        }
        [[nodiscard]] auto getColor() const
        {
            return color;
        }

        // sets font
        // @note triggers onFormat change
        void setFont(RawFont *font)
        {
            setter(this->font, font);
        }
        // sets color
        // @note triggers onFormat change
        void setColor(Color color)
        {
            setter(this->color, color);
        }
    };
}; // namespace gui
