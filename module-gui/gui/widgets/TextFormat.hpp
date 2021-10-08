// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Alignment.hpp"
#include <Color.hpp>
#include <functional>
#include <string>

namespace gui
{
    class RawFont;

    class TextFormat
    {
      private:
        mutable RawFont *font = nullptr;
        Color color = ColorFullBlack;
        Alignment alignment   = Alignment(Alignment::Horizontal::Left);

        static constexpr auto setter = [](auto &local, auto &next) {
            if (local != next) {
                local = next;
            }
        };

      public:
        TextFormat(const RawFont *font, Color color = {}) : font(const_cast<RawFont *>(font)), color(color){};
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

        void setAlignment(Alignment alignment)
        {
            setter(this->alignment, alignment);
        }

        auto str() const -> std::string;
    };
}; // namespace gui
