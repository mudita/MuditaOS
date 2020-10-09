#pragma once

#include <vector>

#include <module-utils/math/Math.hpp>

#include "DrawCommand.hpp"
#include "Context.hpp"

namespace gui
{
    class RawFont;
    class FontGlyph;

    enum class LineExpansionDirection
    {
        LINE_EXPAND_UP    = 1,
        LINE_EXPAND_DOWN  = 1 << 1,
        LINE_EXPAND_LEFT  = 1 << 2,
        LINE_EXPAND_RIGHT = 1 << 3
    };

    enum class Line45degEnd
    {
        TOP_FLAT    = 1,
        TOP_TIP     = 1 << 1,
        BOTTOM_FLAT = 1 << 2,
        BOTTOM_TIP  = 1 << 3
    };

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
        void drawPixel(Context *ctx, Point point, Color color);

        void drawLine(Context *ctx, CommandLine *cmd);

        void drawLine(Context *ctx, Point start, Point end, Color color);

        void drawHorizontalLine(Context *ctx,
                                int16_t x,
                                int16_t y,
                                uint16_t width,
                                uint16_t penWidth,
                                Color color,
                                LineExpansionDirection dir);

        void drawVerticalLine(Context *ctx,
                              int16_t x,
                              int16_t y,
                              uint16_t height,
                              uint16_t penWidth,
                              Color color,
                              LineExpansionDirection dir);

        /**
         * @brief Function for drawing a line skewed at 45 deg with given width in specified direction. by default the
         * line is drawn from top to bottom in V-dim and from left to right in H-dim.
         * @param[in] toRight changes H-dim draw direction, so it is drawn to the right of vertical axis.
         */
        void draw45degLine(Context *ctx,
                           int16_t x,
                           int16_t y,
                           uint16_t side,
                           uint16_t penWidth,
                           Color color,
                           LineExpansionDirection dir,
                           bool toRight,
                           Line45degEnd lineEnds = Line45degEnd::TOP_FLAT | Line45degEnd::BOTTOM_FLAT);

        void drawRectangle(Context *ctx, CommandRectangle *cmd);

        void drawArc(Context *ctx, CommandArc *cmd);

        void drawArc(Context *ctx,
                     Point center,
                     Length radius,
                     trigonometry::Degrees begin,
                     trigonometry::Degrees sweep,
                     Color color);

        void drawArc(Context *ctx,
                     Point center,
                     Length radius,
                     trigonometry::Degrees begin,
                     trigonometry::Degrees sweep,
                     Color color,
                     int width);

        void drawCircle(Context *ctx, CommandCircle *cmd);

        void drawCircle(Context *ctx, Point center, Length radius, Color color, int width);

        void drawFilledCircle(
            Context *ctx, Point center, Length radius, Color borderColor, int borderWidth, Color fillColor);

        void drawText(Context *ctx, CommandText *cmd);

        void drawChar(Context *context, const int16_t x, const int16_t y, FontGlyph *glyph, const Color color);

        void drawImage(Context *ctx, CommandImage *cmd);

      public:
        virtual ~Renderer() = default;

        void render(Context *ctx, std::vector<DrawCommand *> &commands);
    };

} /* namespace gui */
