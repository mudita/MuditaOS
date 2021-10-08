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
    class CircleRenderer
    {
      public:
        struct DrawableStyle
        {
            Length penWidth{1};
            Color borderColor{ColorFullBlack};
            Color fillColor{ColorNoColor};

            static auto from(const DrawCircle &command) -> DrawableStyle;
        };

        static void draw(Context *ctx, Point center, Length radius, const DrawableStyle &style);

      private:
        static void draw(Context *ctx, Point center, Length radius, Color color, Length width);

        static void draw(
            Context *ctx, Point center, Length radius, Color borderColor, Length borderWidth, Color fillColor);
    };
} // namespace gui::renderer
