/*
 * Renderer.hpp
 *
 *  Created on: 7 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_RENDERER_HPP_
#define GUI_CORE_RENDERER_HPP_

#include <vector>
#include "DrawCommand.hpp"
#include "Context.hpp"
#include "Font.hpp"

namespace gui {

    enum class LineExpansionDirection
    {
        LINE_EXPAND_UP = 1,
        LINE_EXPAND_DOWN = 1 << 1,
        LINE_EXPAND_LEFT = 1 << 2,
        LINE_EXPAND_RIGHT = 1 << 3
    };

    enum class Line45degEnd
    {
        TOP_FLAT = 1,
        TOP_TIP = 1 << 1,
        BOTTOM_FLAT = 1 << 2,
        BOTTOM_TIP = 1 << 3
    };

    class Renderer
    {
        /**
         * convention: all the coordinates are is a pixel. the origin is (0,0) and it's ABOVE (UP) & TO THE LEFT of the topmost left pixel.
         * Therefore to draw 1px wide line spanning the whole topmost row, it will be drawHorizontal (0,0, 1px, length, ::DOWN). It's equvalent to
         * drawHorizontal (0,1, 1px, length, ::UP); default is DOWN & RIGHT a is the first pixel. a is (0,0) DOWN (& RIGHT) == a is (0,1) UP (& RIGHT) b is
         * (2,2) UP (& RIGHT)   == b is (2,1) DOWN (& RIGHT)
         *
         * _| 0 1 2 3
         * 0  a
         * 1      b
         * 2
         * 3
         *
         */
      protected:
        /**
         * @brief Function for drawing horizontal line with given width in specified direction
         */
        void drawHorizontalLine(Context *ctx, int16_t x, int16_t y, uint16_t width, uint16_t penWidth, Color color, LineExpansionDirection dir);
        /**
         * @brief Function for drawing vertical line with given width in specified direction
         */
        void drawVerticalLine(Context *ctx, int16_t x, int16_t y, uint16_t height, uint16_t penWidth, Color color, LineExpansionDirection dir);
        /**
         * @brief Function for drawing a line skewed at 45 deg with given width in specified direction. by default the line is drawn from top to bottom in V-dim
         * and from left to right in H-dim.
         * @param[in] toRight changes H-dim draw direction, so it is drawn to the right of vertical axis.
         */
        void draw45degLine(Context *ctx, int16_t x, int16_t y, uint16_t side, uint16_t penWidth, Color color, LineExpansionDirection dir, bool toRight,
                           Line45degEnd lineEnds = Line45degEnd::TOP_FLAT | Line45degEnd::BOTTOM_FLAT);
        /**
         * @brief Function that draws line on the given context
         */
        void drawLine(Context *ctx, CommandLine *cmd);
        /**
         * @brief Function draws rectangle on the given context;
         */
        void drawRectangle(Context *ctx, CommandRectangle *cmd);
        /**
         * @brief Function draws line of text with specified font on the given context;
         */
        void drawText(Context *ctx, CommandText *cmd);
        /**
         * @brief Draws single glyph to specified context
         */
        void drawChar(Context *context, const int16_t x, const int16_t y, Font *font, FontGlyph *glyph, const Color color);
        /**
         * @brief Draws an image with specified ID on given coordinates.
         */
        void drawImage(Context *ctx, CommandImage *cmd);

      public:
        Renderer();
        virtual ~Renderer();

        void render(Context *ctx, std::vector<DrawCommand *> &commands);
};

} /* namespace gui */

#endif /* GUI_CORE_RENDERER_HPP_ */
