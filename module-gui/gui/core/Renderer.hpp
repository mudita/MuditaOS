// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include <module-utils/math/Math.hpp>

#include "DrawCommand.hpp"
#include "Context.hpp"

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
      protected:
        void drawLine(Context *ctx, CommandLine *cmd);

        void drawRectangle(Context *ctx, CommandRectangle *cmd);

        void drawArc(Context *ctx, CommandArc *cmd);

        void drawCircle(Context *ctx, CommandCircle *cmd);

        void drawText(Context *ctx, CommandText *cmd);

        void drawChar(Context *context, const int16_t x, const int16_t y, FontGlyph *glyph, const Color color);

        void drawImage(Context *ctx, CommandImage *cmd);

      public:
        virtual ~Renderer() = default;

        void render(Context *ctx, std::vector<DrawCommand *> &commands);
    };

} /* namespace gui */
