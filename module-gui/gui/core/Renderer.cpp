extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

// for memset
#include <string.h>
#include "Color.hpp"
#include "Renderer.hpp"
#include "Context.hpp"
#include "ImageManager.hpp"
#include "../Common.hpp"
// utils
#include "log/log.hpp"

#include "PixMap.hpp"
#include "VecMap.hpp"
// module-utils
#include "utf8/UTF8.hpp"
#include <cassert>
#include <FontManager.hpp>
#include <FontGlyph.hpp>
#include <RawFont.hpp>

#if DEBUG_FONT == 1
#define log_warn_glyph(...) LOG_WARN(__VA_ARGS__)
#else
#define log_warn_glyph(...)
#endif

namespace gui
{

    void Renderer::drawLine(Context *ctx, CommandLine *cmd)
    {}

    // any checks are not performed as this should be done by function handling draw command
    void Renderer::drawHorizontalLine(
        Context *ctx, int16_t x, int16_t y, uint16_t width, uint16_t penWidth, Color color, LineExpansionDirection dir)
    {

        // if color is fully transparent - return
        if (color.alpha == 0x0F)
            return;

        uint32_t drawOffset = y * ctx->getW() + x;
        int32_t rowStride;

        // line can be expanded up or down, any other option will cause function to return
        if (dir == LineExpansionDirection::LINE_EXPAND_DOWN) {
            rowStride = ctx->getW();
        }
        else if (dir == LineExpansionDirection::LINE_EXPAND_UP) {
            rowStride = -ctx->getW();
            drawOffset += rowStride;
        }
        else
            return;

        for (uint32_t i = 0; i < penWidth; i++) {
            memset(ctx->getData() + drawOffset, color.intensity, width);
            drawOffset += rowStride;
        }
    }

    // any checks are not performed as this should be done by function handling draw command
    void Renderer::drawVerticalLine(
        Context *ctx, int16_t x, int16_t y, uint16_t height, uint16_t penWidth, Color color, LineExpansionDirection dir)
    {

        // if color is fully transparent - return
        if (color.alpha == 0x0F)
            return;

        uint32_t drawOffset = y * ctx->getW() + x;
        if (dir == LineExpansionDirection::LINE_EXPAND_RIGHT) {
            // no action needed unless there is need to draw gradient
        }
        else if (dir == LineExpansionDirection::LINE_EXPAND_LEFT) {
            drawOffset -= penWidth;
        }
        else {
            return;
        }

        int32_t rowStride = ctx->getW();

        for (uint32_t row = 0; row < height; row++) {
            memset(ctx->getData() + drawOffset, color.intensity, penWidth);
            drawOffset += rowStride;
        }
    }

    void Renderer::draw45degLine(Context *ctx,
                                 int16_t x,
                                 int16_t y,
                                 uint16_t side,
                                 uint16_t penWidth,
                                 Color color,
                                 LineExpansionDirection dir,
                                 bool toRight,
                                 Line45degEnd lineEnds)
    {
        // if color is fully transparent - return
        if (color.alpha == 0x0F) {
            return;
        }

        uint16_t penWidthAcross = 0;
        switch (penWidth) {
        case 1:
            penWidthAcross = 1;
            break;
        case 2:
            penWidthAcross = 3;
            break;
        case 3:
            penWidthAcross = 5;
            break;
        default:
            penWidthAcross = penWidth * 141 / 100;
            break;
        }

        // approach: as in drawVertical rather than drawHorizontal. draw small horizontal lines offset to preceding one
        uint32_t drawOffset = y * ctx->getW() + x;
        if (dir & LineExpansionDirection::LINE_EXPAND_RIGHT) {
            // no action needed unless there is need to draw gradient
        }
        else if (dir & LineExpansionDirection::LINE_EXPAND_LEFT) {
            drawOffset -= penWidthAcross;
        }

        int32_t rowStride = ctx->getW();
        if (dir & LineExpansionDirection::LINE_EXPAND_DOWN) {}
        else if (dir & LineExpansionDirection::LINE_EXPAND_UP) {
            drawOffset -= rowStride * penWidth;
        }
        // memsets are vertical. they fill a column

        // produces top tip
        if (lineEnds & Line45degEnd::TOP_TIP) {
            drawOffset -= rowStride * penWidthAcross;
            for (uint32_t skew = 0; skew < penWidthAcross; skew++) {
                if (toRight) {
                    memset(ctx->getData() + drawOffset, color.intensity, skew);
                }
                else {
                    memset(ctx->getData() + drawOffset + (penWidthAcross - skew), color.intensity, (skew));
                }
                drawOffset += rowStride;
            }
        }
        // produces \\\ with flat top and bottom
        for (uint32_t skew = 0; skew < side; skew++) {
            memset(ctx->getData() + drawOffset, color.intensity, penWidthAcross);
            drawOffset += rowStride;
            drawOffset += (toRight ? 1 : -1); // add skew
        }
        // produces bottom tip
        if (lineEnds & Line45degEnd::BOTTOM_TIP) {
            for (uint32_t skew = 0; skew < penWidthAcross; skew++) {
                if (toRight) {
                    memset(ctx->getData() + (drawOffset + skew), color.intensity, (penWidthAcross - skew) - 1);
                }
                else {
                    memset(ctx->getData() + drawOffset + 1, color.intensity, (penWidthAcross - skew) - 1);
                }
                drawOffset += rowStride;
            }
        }
    }

    void Renderer::drawRectangle(Context *ctx, CommandRectangle *cmd)
    {

        // check if there is anything to draw
        if (cmd->w == 0 || cmd->h == 0) {
            return;
        }

        if (((cmd->fillColor.alpha == 0x0F) && (cmd->borderColor.alpha == 0x0F)) ||
            ((cmd->filled == false) && (cmd->borderColor.alpha == 0x0F))) {
            return;
        }
        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context *drawCtx;
        bool copyContext = false;
        int16_t wgtX = 0, wgtY = 0;
        uint16_t wgtW = cmd->areaW, wgtH = cmd->areaH;
        int16_t wgtR = cmd->radius;

        if (cmd->yaps & (RectangleYapFlags::GUI_RECT_YAP_BOTTOM_LEFT | RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT)) {
            wgtX += cmd->yapSize;
            wgtW -= cmd->yapSize;
        }
        if (cmd->yaps & (RectangleYapFlags::GUI_RECT_YAP_BOTTOM_RIGHT | RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT)) {
            wgtW -= cmd->yapSize;
        }

        // check if there is a need or making copy of context to use is as background
        if ((cmd->areaW == cmd->w) && (cmd->areaH == cmd->h)) {
            drawCtx = ctx;
            wgtX += cmd->x;
            wgtY += cmd->y;
        }
        else {
            copyContext = true;
            int16_t x   = cmd->x;
            int16_t y   = cmd->y;
            if (cmd->areaX < 0)
                x += cmd->areaX;
            if (cmd->areaY < 0)
                y += cmd->areaY;
            drawCtx = ctx->get(x, y, cmd->areaW, cmd->areaH);
        }

        // if rounding of corners is 0
        if (wgtR == 0) {
            // fill field inside the rectangle if fill flag is set
            if (cmd->filled) {
                uint32_t offset = wgtY * drawCtx->getW() + wgtX;
                for (int32_t y = 0; y < cmd->areaH; y++) {
                    memset(drawCtx->getData() + offset, cmd->fillColor.intensity, wgtW);
                    offset += drawCtx->getW();
                }
            }
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP)
                drawHorizontalLine(drawCtx,
                                   wgtX,
                                   wgtY,
                                   wgtW,
                                   cmd->penWidth,
                                   cmd->borderColor,
                                   LineExpansionDirection::LINE_EXPAND_DOWN);
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM)
                drawHorizontalLine(drawCtx,
                                   wgtX,
                                   wgtY + wgtH,
                                   wgtW,
                                   cmd->penWidth,
                                   cmd->borderColor,
                                   LineExpansionDirection::LINE_EXPAND_UP);
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT)
                drawVerticalLine(drawCtx,
                                 wgtX,
                                 wgtY,
                                 wgtH,
                                 cmd->penWidth,
                                 cmd->borderColor,
                                 LineExpansionDirection::LINE_EXPAND_RIGHT);
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT)
                drawVerticalLine(drawCtx,
                                 wgtX + wgtW,
                                 wgtY,
                                 wgtH,
                                 cmd->penWidth,
                                 cmd->borderColor,
                                 LineExpansionDirection::LINE_EXPAND_LEFT);
        }
        else {

            // calculate centers of circle for all corners
            int16_t xcTopRight    = wgtX + wgtW - wgtR;
            int16_t xcTopLeft     = wgtX + wgtR;
            int16_t xcBottomRight = xcTopRight;
            int16_t xcBottomLeft  = xcTopLeft;

            int16_t ycTopRight    = wgtY + wgtR;
            int16_t ycTopLeft     = ycTopRight;
            int16_t ycBottomRight = wgtY + cmd->h - wgtR;
            int16_t ycBottomLeft  = ycBottomRight;

            int x = wgtR, y = 0;

            // Calculate coordinates of the points on the circle's quarter.
            std::unique_ptr<int16_t[]> Px, Py;
            try {
                Px = std::unique_ptr<int16_t[]>(new int16_t[2 * wgtR * sizeof(int16_t)]);
                Py = std::unique_ptr<int16_t[]>(new int16_t[2 * wgtR * sizeof(int16_t)]);
            }
            catch (const std::bad_alloc &) {
                LOG_ERROR("Allocation error");
                return;
            }

            memset(Px.get(), 0, 2 * wgtR * sizeof(int16_t));
            memset(Py.get(), 0, 2 * wgtR * sizeof(int16_t));
            int32_t index = 0;
            // index where middle point of quarter of arc is located. This means that Y starts to grow faster than X
            int32_t middleIndex = 0;
            // number of points in the quater arc
            int32_t pointCount = 0;
            Px[index]          = x;
            Py[index]          = y;
            ++index;
            pointCount++;

            int P = 1 - wgtR;

            while (y < wgtR) // just a failsafe
            {
                y++;

                // Mid-point is inside or on the perimeter
                if (P <= 0)
                    P = P + 2 * y + 1;

                // Mid-point is outside the perimeter
                else {
                    x--;
                    P = P + 2 * y - 2 * x + 1;
                }

                Px[index] = x;
                Py[index] = y;

                // All the perimeter points have already been printed
                if (x <= y) {

                    int16_t pointsLeft = 0;
                    int indexOut = index;
                    if (x == y) {
                        // we have a central point. it can be drawn with either type of line
                        middleIndex = index;
                        pointsLeft  = index;
                        pointsLeft++;
                    }
                    else if (x < y) {
                        pointsLeft = index;
                        // we don't have a central point. halves are alike
                        index--;
                        middleIndex = index;
                    }
                    int xValue = 0; // Px[index];

                    while (pointsLeft > 0) {
                        Px[indexOut] = Py[index];
                        Py[indexOut] = Px[index] - 2 * xValue;
                        index--;
                        indexOut++;
                        pointCount++;
                        pointsLeft--;
                    }
                    break;
                }
                ++index;
                pointCount++;
            }

            // fill the rectangle if fill flag is set using fillColor. Painting is divieded into 3 parts
            // First part takes into consideration if top corners exists, are they rounded or are they sharp.
            // Second draws simple lines between between left and right edges.
            // Third part takes into consideration if bottom corners exists, are they rounded or are they sharp.

            if (cmd->filled) {
                // corners
                if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT) {
                    // if corner then evaluate: sharp corner (aka flat), sms (aka yap), else: round.
                    if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT) {}
                    else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT) {
                    }
                    else {
                        // draw arc from 0 index up to mid point using horizontal line
                        index = 0;
                        // X is growing faster
                        for (index = 1; index < middleIndex; ++index) {
                            // then draw the outline
                            // fill the quarter first
                            drawHorizontalLine(drawCtx,
                                               xcTopRight,
                                               ycTopRight - Py[index],
                                               Px[index],
                                               1,
                                               cmd->fillColor,
                                               gui::LineExpansionDirection::LINE_EXPAND_DOWN);
                            ;
                        }
                        // Y is growing faster
                        for (index = middleIndex; index < pointCount - 1; ++index) {
                            drawVerticalLine(drawCtx,
                                             xcTopRight + Px[index],
                                             ycTopRight - Py[index] + cmd->penWidth,
                                             Py[index] - cmd->penWidth,
                                             1,
                                             cmd->fillColor,
                                             gui::LineExpansionDirection::LINE_EXPAND_LEFT);
                        }
                    }
                }
                if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_RIGHT) {
                    if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT) {}
                    else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_RIGHT) {
                    }
                    else {
                        // draw arc from 0 index up to mid point using horizontal line
                        index = 0;
                        // X is growing faster
                        for (index = 1; index < middleIndex; ++index) {
                            // fill the quarter first
                            drawHorizontalLine(drawCtx,
                                               xcBottomRight,
                                               ycBottomRight + Py[index],
                                               Px[index] - cmd->penWidth,
                                               1,
                                               cmd->fillColor,
                                               gui::LineExpansionDirection::LINE_EXPAND_UP);
                        }
                        // Y is growing faster
                        for (index = middleIndex; index < pointCount - 1; ++index) {
                            drawVerticalLine(drawCtx,
                                             xcBottomRight + Px[index],
                                             ycBottomRight /*+ 1*/,
                                             Py[index],
                                             1,
                                             cmd->fillColor,
                                             gui::LineExpansionDirection::LINE_EXPAND_LEFT);
                        }
                    }
                }
                if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_LEFT) {
                    if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT) {}
                    else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT) {
                    }
                    else {
                        // draw arc from 0 index up to mid point using horizontal line
                        index = 0;
                        // X is growing faster
                        for (index = 1; index < middleIndex; ++index) {
                            // fill the quarter first
                            drawHorizontalLine(drawCtx,
                                               xcTopLeft - Px[index] + cmd->penWidth,
                                               ycTopLeft - Py[index],
                                               Px[index] - cmd->penWidth,
                                               1,
                                               cmd->fillColor,
                                               gui::LineExpansionDirection::LINE_EXPAND_DOWN);
                        }
                        // Y is growing faster
                        for (index = middleIndex; index < pointCount - 1; ++index) {
                            drawVerticalLine(drawCtx,
                                             xcTopLeft - Px[index] /*+ 1*/,
                                             ycTopLeft - Py[index] + cmd->penWidth,
                                             Py[index] - cmd->penWidth,
                                             1,
                                             cmd->fillColor,
                                             gui::LineExpansionDirection::LINE_EXPAND_RIGHT);
                        }
                    }
                }
                if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_LEFT) {
                    if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT) {}
                    else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_LEFT) {
                    }
                    else {
                        // draw arc from 0 index up to mid point using horizontal line
                        index = 0;
                        // X is growing faster
                        for (index = 1; index < middleIndex; ++index) {
                            // fill the quarter first
                            drawHorizontalLine(drawCtx,
                                               xcBottomLeft - Px[index] + cmd->penWidth,
                                               ycBottomLeft + Py[index],
                                               Px[index] - cmd->penWidth,
                                               1,
                                               cmd->fillColor,
                                               gui::LineExpansionDirection::LINE_EXPAND_UP);
                        }

                        ;
                        // Y is growing faster
                        for (index = middleIndex; index < pointCount - 1; ++index) {
                            drawVerticalLine(drawCtx,
                                             xcBottomLeft - Px[index],
                                             ycBottomLeft,
                                             Py[index] - cmd->penWidth,
                                             1,
                                             cmd->fillColor,
                                             gui::LineExpansionDirection::LINE_EXPAND_RIGHT);
                        }
                    }
                }

                // edges
                if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP) {
                    drawHorizontalLine(drawCtx,
                                       wgtX + wgtR,
                                       wgtY,
                                       wgtW - 2 * wgtR,
                                       wgtR,
                                       cmd->fillColor,
                                       LineExpansionDirection::LINE_EXPAND_DOWN);
                }
                if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM) {
                    drawHorizontalLine(drawCtx,
                                       wgtX + wgtR,
                                       wgtY + wgtH - wgtR,
                                       wgtW - 2 * wgtR,
                                       wgtR,
                                       cmd->fillColor,
                                       LineExpansionDirection::LINE_EXPAND_DOWN);
                }
                if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT) {
                    drawHorizontalLine(drawCtx,
                                       wgtX,
                                       wgtY + wgtR,
                                       wgtR,
                                       wgtH - 2 * wgtR,
                                       cmd->fillColor,
                                       LineExpansionDirection::LINE_EXPAND_DOWN);
                }
                if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT) {
                    drawHorizontalLine(drawCtx,
                                       wgtX + wgtW - wgtR,
                                       wgtY + wgtR,
                                       wgtR,
                                       wgtH - 2 * wgtR,
                                       cmd->fillColor,
                                       LineExpansionDirection::LINE_EXPAND_DOWN);
                }

                // center
                drawHorizontalLine(drawCtx,
                                   wgtX + wgtR,
                                   wgtY + wgtR,
                                   wgtW - 2 * wgtR,
                                   wgtH - 2 * wgtR,
                                   cmd->fillColor,
                                   LineExpansionDirection::LINE_EXPAND_DOWN);
            }

            // Here rounded border is painted

            // upper right corner
            if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT) {
                // if corner then evaluate: sharp corner (aka flat), sms (aka yap), else: round.
                if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT) {}
                else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT) {
                }
                else {
                    // draw arc from 0 index up to mid point using horizontal line
                    index = 0;
                    // X is growing faster
                    for (index = 0; index < middleIndex; ++index) {
                        // then draw the outline
                        drawHorizontalLine(drawCtx,
                                           xcTopRight + Px[index] - cmd->penWidth,
                                           ycTopRight - Py[index],
                                           cmd->penWidth,
                                           1,
                                           cmd->borderColor,
                                           gui::LineExpansionDirection::LINE_EXPAND_DOWN);
                    }
                    // Y is growing faster
                    for (index = middleIndex; index < pointCount; ++index) {
                        drawVerticalLine(drawCtx,
                                         xcTopRight + Px[index],
                                         ycTopRight - Py[index],
                                         cmd->penWidth,
                                         1,
                                         cmd->borderColor,
                                         gui::LineExpansionDirection::LINE_EXPAND_LEFT);

                        ;
                    }
                }
            }

            // bottom right corner
            if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_RIGHT) {
                if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT) {}
                else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_RIGHT) {
                }
                else {
                    // draw arc from 0 index up to mid point using horizontal line
                    index = 0;
                    // X is growing faster
                    for (index = 0; index < middleIndex; ++index) {
                        drawHorizontalLine(drawCtx,
                                           xcBottomRight + Px[index] - cmd->penWidth,
                                           ycBottomRight + Py[index],
                                           cmd->penWidth,
                                           1,
                                           cmd->borderColor,
                                           gui::LineExpansionDirection::LINE_EXPAND_UP);
                    }
                    // Y is growing faster
                    for (index = middleIndex; index < pointCount; ++index) {
                        drawVerticalLine(drawCtx,
                                         xcBottomRight + Px[index],
                                         ycBottomRight + Py[index] - cmd->penWidth /*+ 1*/,
                                         cmd->penWidth,
                                         1,
                                         cmd->borderColor,
                                         gui::LineExpansionDirection::LINE_EXPAND_LEFT);
                    }
                }
            }

            // upper left corner
            if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_LEFT) {
                if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT) {}
                else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT) {
                }
                else {
                    // draw arc from 0 index up to mid point using horizontal line
                    index = 0;
                    // X is growing faster
                    for (index = 0; index < middleIndex; ++index) {
                        drawHorizontalLine(drawCtx,
                                           xcTopLeft - Px[index],
                                           ycTopLeft - Py[index],
                                           cmd->penWidth,
                                           1,
                                           cmd->borderColor,
                                           gui::LineExpansionDirection::LINE_EXPAND_DOWN);
                    }
                    // Y is growing faster
                    for (index = middleIndex; index < pointCount; ++index) {
                        drawVerticalLine(drawCtx,
                                         xcTopLeft - Px[index] /*+ 1*/,
                                         ycTopLeft - Py[index],
                                         cmd->penWidth,
                                         1,
                                         cmd->borderColor,
                                         gui::LineExpansionDirection::LINE_EXPAND_RIGHT);
                    }
                }
            }

            // lower left corner
            if (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_LEFT) {
                if (cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT) {}
                else if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_LEFT) {
                }
                else {
                    // draw arc from 0 index up to mid point using horizontal line
                    index = 0;
                    // X is growing faster
                    for (index = 0; index < middleIndex; ++index) {

                        drawHorizontalLine(drawCtx,
                                           xcBottomLeft - Px[index],
                                           ycBottomLeft + Py[index],
                                           cmd->penWidth,
                                           1,
                                           cmd->borderColor,
                                           gui::LineExpansionDirection::LINE_EXPAND_UP);
                    }
                    // Y is growing faster
                    for (index = middleIndex; index < pointCount; ++index) {

                        drawVerticalLine(drawCtx,
                                         xcBottomLeft - Px[index],
                                         ycBottomLeft + Py[index] - cmd->penWidth,
                                         cmd->penWidth,
                                         1,
                                         cmd->borderColor,
                                         gui::LineExpansionDirection::LINE_EXPAND_RIGHT);
                    }
                }
            }

            // render edges between corners
            int16_t xs, ys, le;
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP) {
                le = wgtW;
                // left "corner" first
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT) {
                    xs = -cmd->yapSize;
                    xs += cmd->penWidth;
                }
                else {
                    xs = wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT));
                }
                // right "corner" now
                le -= xs;
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT) {
                    le += (cmd->yapSize - cmd->penWidth);
                }
                else {
                    le -= wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT));
                }
                ys = 0;
                drawHorizontalLine(drawCtx,
                                   wgtX + xs,
                                   wgtY + ys,
                                   le,
                                   cmd->penWidth,
                                   cmd->borderColor,
                                   LineExpansionDirection::LINE_EXPAND_DOWN);
            }
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM) {
                le = wgtW;
                // left "corner" first
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_LEFT) {
                    xs = -cmd->yapSize;
                    xs += cmd->penWidth;
                }
                else {
                    xs = wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT));
                }
                // right "corner" now
                le -= xs;
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_RIGHT) {
                    le += (cmd->yapSize - cmd->penWidth);
                }
                else {
                    le -= wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT));
                }
                ys = wgtH; // beware. doesn't -1 break rounded corners ?
                drawHorizontalLine(drawCtx,
                                   wgtX + xs,
                                   wgtY + ys,
                                   le,
                                   cmd->penWidth,
                                   cmd->borderColor,
                                   LineExpansionDirection::LINE_EXPAND_UP);
            }

            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT) {
                // top "corner" first
                xs = 0;
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT) {
                    ys = cmd->yapSize;
                    draw45degLine(drawCtx,
                                  wgtX + xs - cmd->yapSize,
                                  wgtY + ys - cmd->yapSize,
                                  cmd->yapSize,
                                  cmd->penWidth,
                                  cmd->borderColor,
                                  LineExpansionDirection::LINE_EXPAND_RIGHT,
                                  true,
                                  Line45degEnd::BOTTOM_TIP);
                }
                else {
                    ys = wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT));
                }
                // bottom "corner" now
                le = wgtH - ys;

                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_LEFT) {
                    le -= cmd->yapSize;
                    draw45degLine(drawCtx,
                                  wgtX + xs,
                                  wgtY + ys + le,
                                  cmd->yapSize,
                                  cmd->penWidth,
                                  cmd->borderColor,
                                  LineExpansionDirection::LINE_EXPAND_RIGHT,
                                  false,
                                  Line45degEnd::TOP_TIP);
                }
                else {
                    le -= wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT));
                }
                drawVerticalLine(drawCtx,
                                 wgtX + xs,
                                 wgtY + ys,
                                 le,
                                 cmd->penWidth,
                                 cmd->borderColor,
                                 LineExpansionDirection::LINE_EXPAND_RIGHT);
            }
            if (cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT) {
                // top "corner" first
                xs = wgtW;
                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT) {
                    ys = cmd->yapSize;
                    draw45degLine(drawCtx,
                                  wgtX + xs + cmd->yapSize,
                                  wgtY + ys - cmd->yapSize,
                                  cmd->yapSize,
                                  cmd->penWidth,
                                  cmd->borderColor,
                                  LineExpansionDirection::LINE_EXPAND_LEFT,
                                  false,
                                  Line45degEnd::BOTTOM_TIP);
                }
                else {
                    ys = wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT));
                }
                // bottom "corner" now
                le = wgtH - ys;

                if (cmd->yaps & RectangleYapFlags::GUI_RECT_YAP_BOTTOM_RIGHT) {
                    le -= cmd->yapSize;
                    draw45degLine(drawCtx,
                                  wgtX + xs,
                                  wgtY + ys + le,
                                  cmd->yapSize,
                                  cmd->penWidth,
                                  cmd->borderColor,
                                  LineExpansionDirection::LINE_EXPAND_LEFT,
                                  true,
                                  Line45degEnd::TOP_TIP);
                }
                else {
                    le -= wgtR * (!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT));
                }
                drawVerticalLine(drawCtx,
                                 wgtX + xs,
                                 wgtY + ys,
                                 le,
                                 cmd->penWidth,
                                 cmd->borderColor,
                                 LineExpansionDirection::LINE_EXPAND_LEFT);
            }
        }

        // if drawing was performed in temporary context
        // reinsert drawCtx into last context
        if (copyContext) {
            ctx->insertArea(cmd->x, cmd->y, cmd->areaX, cmd->areaY, cmd->w, cmd->h, drawCtx);
            delete drawCtx;
        }
    }

    void Renderer::drawChar(
        Context *context, const int16_t x, const int16_t y, RawFont *font, FontGlyph *glyph, const Color color)
    {
        auto line_y_offset = (y - glyph->yoffset) * context->getW();
        auto *drawPtr      = context->getData() + x + line_y_offset;
        auto *glyphPtr     = glyph->data;
        assert(glyph->data);

        for (uint16_t yy = 0; yy < glyph->height; ++yy) {
            for (uint16_t xx = 0; xx < glyph->width; ++xx) {
                if (!context->addressInData(drawPtr + xx)) {
                    log_warn_glyph("drawing out of: %d vs %d", xx, context->getW() * context->getH());
                    return;
                }
                if (*(glyphPtr + xx) == 0) {
                    *(drawPtr + xx) = 0x0F & color.intensity;
                }
            }
            drawPtr += context->getW();
            glyphPtr += glyph->width;
        }
    }

    void Renderer::drawText(Context *ctx, CommandText *cmd)
    {

        // check if there are any characters to draw in the string provided with message.
        if (cmd->str.length() == 0) {
            return;
        }

        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context *drawCtx;
        bool copyContext = false;
        int16_t wgtX = 0, wgtY = 0;
        // check if there is a need or making copy of context to use is as background
        if ((cmd->areaW == cmd->w) && (cmd->areaH == cmd->h)) {
            drawCtx = ctx;
            wgtX    = cmd->x;
            wgtY    = cmd->y;
        }
        else {
            copyContext = true;
            drawCtx     = ctx->get(cmd->x, cmd->y, cmd->areaW, cmd->areaH);
        }

        // retrieve font used to draw text
        FontManager &fontManager = FontManager::getInstance();
        RawFont *font            = fontManager.getFont(cmd->fontID);

        int16_t posX = cmd->tx;
        int16_t posY = cmd->ty;

        // draw every sign
        uint32_t idLast = 0, idCurrent = 0;
        for (uint32_t i = 0; i < cmd->str.length(); ++i) {
            idCurrent        = cmd->str[i]; // id stands for glued together utf-16 with no order bytes (0xFF 0xFE)
            auto glyph_found = font->glyphs.find(idCurrent);

            std::unique_ptr<FontGlyph> unique_glyph;

            FontGlyph *glyph = nullptr;
            if (glyph_found != font->glyphs.end()) {
                glyph = glyph_found->second;
            }
            else {
                log_warn_glyph(
                    "no glyph for character id:%" PRIu32 " in font \"%s\"", idCurrent, font->info.face.c_str());
                unique_glyph = font->getGlyphUnsupported();
                glyph        = unique_glyph.get();
            }

            // do not start drawing outside of draw context.
            if ((wgtX + posX + glyph->xoffset >= drawCtx->getW()) || (wgtX + posX + glyph->xoffset < 0)) {
                LOG_FATAL("Drawing outside context's X boundary for glyph: %" PRIu32, glyph->id);
                return;
            }
            if ((wgtY + posY >= drawCtx->getH()) || (wgtY + posY < 0)) {
                LOG_FATAL("Drawing outside context's Y boundary for glyph: %" PRIu32, glyph->id);
                return;
            }

            int32_t kernValue = 0;
            if (i > 0) {
                kernValue = font->getKerning(idLast, idCurrent);
            }
            drawChar(drawCtx, wgtX + posX + glyph->xoffset + kernValue, wgtY + posY, font, glyph, cmd->color);
            posX += glyph->xadvance + kernValue;

            idLast = idCurrent;
        }

        // if drawing was performed in temporary context
        // reinsert drawCtx into bast context
        if (copyContext) {
            ctx->insert(cmd->x, cmd->y, drawCtx);
            delete drawCtx;
        }
    }

    void Renderer::drawImage(Context *ctx, CommandImage *cmd)
    {

        // retrive pixmap from the pixmap manager
        ImageMap *imageMap = ImageManager::getInstance().getImageMap(cmd->imageID);

        // if image is not found return;
        if (imageMap == nullptr)
            return;

        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context *drawCtx = ctx->get(cmd->x, cmd->y, cmd->areaW, cmd->areaH);
        uint8_t *ctxData = drawCtx->getData();

        auto check_wh = [&](const std::string &name, auto w, auto h) {
            if (h > ctx->getH() || w > ctx->getW()) {
                LOG_WARN("image %s {w: %d,h %d} > context {w %d,h %d}",
                         name.c_str(),
                         w,
                         drawCtx->getW(),
                         h,
                         drawCtx->getH());
            }
        };

        if (imageMap->getType() == gui::ImageMap::Type::PIXMAP) {
            auto pixMap = dynamic_cast<PixMap *>(imageMap);
            assert(pixMap);
            uint32_t offsetImage   = 0;
            uint32_t offsetContext = 0;
            uint8_t *pixData       = pixMap->getData();
            check_wh(pixMap->getName(), pixMap->getWidth(), pixMap->getHeight());

            for (uint32_t row = 0; row < std::min(drawCtx->getH(), pixMap->getHeight()); row++) {
                memcpy(ctxData + offsetContext, pixData + offsetImage, std::min(drawCtx->getW(), pixMap->getWidth()));
                offsetImage += pixMap->getWidth();
                offsetContext += drawCtx->getW();
            }
        }
        else if (imageMap->getType() == gui::ImageMap::Type::VECMAP) {
            auto vecMap = dynamic_cast<VecMap *>(imageMap);
            assert(vecMap);
            uint32_t offsetContext    = 0;
            uint32_t offsetRowContext = 0;
            uint32_t imageOffset      = 0;
            uint8_t alphaColor        = vecMap->getAlphaColor();
            for (uint32_t row = 0; row < std::min(vecMap->getHeight(), drawCtx->getH()); row++) {
                check_wh(vecMap->getName(), imageMap->getWidth(), imageMap->getHeight());
                uint16_t vecCount = *(vecMap->getData() + imageOffset);
                imageOffset += sizeof(uint16_t);

                offsetRowContext = offsetContext;

                for (uint32_t vec = 0; vec < vecCount; ++vec) {

                    uint16_t vecOffset = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint16_t);
                    uint16_t vecLength = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint8_t);
                    uint8_t vecColor = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint8_t);

                    offsetRowContext += vecOffset;
                    if (vecColor != alphaColor) {
                        memset(ctxData + offsetRowContext, vecColor, std::min(drawCtx->getW(), vecLength));
                    }
                    offsetRowContext += vecLength;
                }
                offsetContext += drawCtx->getW();
            }
        }

        // reinsert drawCtx into bast context
        ctx->insert(cmd->x, cmd->y, drawCtx);

        // remove draw context
        delete drawCtx;
    }

    void Renderer::render(Context *ctx, std::vector<DrawCommand *> &commands)
    {
        for (DrawCommand *cmd : commands) {
            switch (cmd->id) {
            case DrawCommandID::GUI_DRAW_CLEAR: {
                //				uint32_t start_tick = xTaskGetTickCount();
                ctx->fill(15);
                //				uint32_t end_tick = xTaskGetTickCount();
                //				LOG_INFO("[ServiceGUI] ctx->fill( 15 ); Time: %d", end_tick - start_tick);
            } break;
            case DrawCommandID::GUI_DRAW_LINE: {

            } break;
            case DrawCommandID::GUI_DRAW_RECT: {

                //				uint32_t start_tick = xTaskGetTickCount();
                drawRectangle(ctx, static_cast<CommandRectangle *>(cmd));
                //				uint32_t end_tick = xTaskGetTickCount();
                //				LOG_INFO("[ServiceGUI] drawRect Time: %d", end_tick - start_tick);

            } break;
            case DrawCommandID::GUI_DRAW_TEXT: {

                //				uint32_t start_tick = xTaskGetTickCount();
                drawText(ctx, static_cast<CommandText *>(cmd));
                //				uint32_t end_tick = xTaskGetTickCount();
                //				LOG_INFO("[ServiceGUI] drawText time: %d", end_tick - start_tick);
            } break;
            case DrawCommandID::GUI_DRAW_IMAGE: {

                //				uint32_t start_tick = xTaskGetTickCount();
                drawImage(ctx, static_cast<CommandImage *>(cmd));
                //				uint32_t end_tick = xTaskGetTickCount();
                //				LOG_INFO("[ServiceGUI] drawImage time: %d", end_tick - start_tick);
            } break;
            default:
                break;
            };
        }
    }

} /* namespace gui */
