#pragma once

#include <Color.hpp>
#include <functional>

namespace gui
{
    class Font;

    class TextFormat {
        private:
            Font *font = nullptr;
            Color color = ColorFullBlack;
            std::function<void()> onFormatChanged = nullptr;

            static constexpr auto setter = [](auto &local, auto &next, std::function<void()> cb) {
                if (local != next) {
                    local = next;
                    if (cb) {
                        cb();
                    }
                }
            };

          public:
            TextFormat(Font *font, Color color = {}) : font(font), color(color){};
            TextFormat(const TextFormat &) = default;

            [[nodiscard]] auto getFont() const { return font; }
            [[nodiscard]] auto getColor() const { return color; }

            // sets font
            // @note triggers onFormat change
            void setFont(Font* font) { 
                setter(this->font, font, onFormatChanged);
            }
            // sets color
            // @note triggers onFormat change
            void setColor(Color color)
            {
                setter(this->color, color, onFormatChanged);
            }
    };
};
