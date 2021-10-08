// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Color.hpp"
#include "Common.hpp"

#include "DrawCommand.hpp"

#include <Math.hpp>

namespace gui
{
    class Context;
} // namespace gui

namespace gui::renderer
{
    class ArcRenderer
    {
      public:
        struct DrawableStyle
        {
            Length penWidth{1};
            Color color{ColorFullBlack};

            static auto from(const DrawArc &command) -> DrawableStyle;
        };

        static void draw(Context *ctx,
                         Point center,
                         Length radius,
                         trigonometry::Degrees begin,
                         trigonometry::Degrees sweep,
                         const DrawableStyle &style);

      private:
        static void draw(Context *ctx,
                         Point center,
                         Length radius,
                         trigonometry::Degrees begin,
                         trigonometry::Degrees sweep,
                         Color color);

        static void draw(Context *ctx,
                         Point center,
                         Length radius,
                         trigonometry::Degrees begin,
                         trigonometry::Degrees sweep,
                         Color color,
                         Length width);
    };
} // namespace gui::renderer
