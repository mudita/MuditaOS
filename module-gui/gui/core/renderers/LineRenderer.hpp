// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Color.hpp"
#include "Common.hpp"

#include "DrawCommand.hpp"

namespace gui
{
    class Context;
} // namespace gui

namespace gui::renderer
{
    enum class LineExpansionDirection
    {
        Up    = 1,
        Down  = (1 << 1),
        Left  = (1 << 2),
        Right = (1 << 3)
    };

    class LineRenderer
    {
      public:
        struct DrawableStyle
        {
            Length penWidth{1};
            Color color{ColorFullBlack};
            LineExpansionDirection direction = LineExpansionDirection::Down;

            static auto from(const DrawLine &command) -> DrawableStyle;

            DrawableStyle &setExpansionDirection(LineExpansionDirection value) noexcept
            {
                direction = value;
                return *this;
            }
        };

        static void draw(Context *ctx, Point start, Point end, Color color);

        static void drawHorizontal(Context *ctx, Point start, Length width, const DrawableStyle &style);

        static void drawVertical(Context *ctx, Point start, Length height, const DrawableStyle &style);

        static void draw45deg(Context *ctx, Point start, Length length, const DrawableStyle &style, bool toRight);

      private:
        static void drawSlanting(Context *ctx,
                                 Point start,
                                 Point end,
                                 Length penWidth,
                                 Color color,
                                 LineExpansionDirection expansionDirection);
    };
} // namespace gui::renderer
