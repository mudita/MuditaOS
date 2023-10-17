// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>

#include <Math.hpp>

#include "DrawCommand.hpp"
#include "Context.hpp"
#include "DrawCommandForward.hpp"

namespace gui
{
    class RawFont;
    class FontGlyph;

    class Renderer
    {
        /**
         * convention: all the coordinates are is a pixel. the origin is (0,0) and it's ABOVE (UP) & TO THE LEFT of the
         * topmost left pixel. Therefore to draw 1px wide line spanning the whole topmost row, it will be drawHorizontal
         * (0,0, 1px, length, ::DOWN). It's equvalent to drawHorizontal (0,1, 1px, length, ::UP); default is DOWN &
         * RIGHT a is the first pixel. a is (0,0) DOWN (& RIGHT) == a is (0,1) UP (& RIGHT) b is (2,2) UP (& RIGHT)   ==
         * b is (2,1) DOWN (& RIGHT)
         *
         * _| 0 1 2 3
         * 0  a
         * 1      b
         * 2
         * 3
         *
         */

      public:
        void changeColorScheme(const std::unique_ptr<ColorScheme> &scheme) const;
        void render(Context *ctx, const std::list<std::unique_ptr<DrawCommand>> &commands) const;

        template <typename... Commands>
        void render(Context &ctx, const Commands &...commands) const
        {
            (..., commands.draw(&ctx));
        }
    };

} /* namespace gui */
