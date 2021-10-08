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
    class RectangleRenderer
    {
      public:
        struct DrawableStyle
        {
            Length borderWidth{1};
            Length radius{0};
            Length yapSize{0};
            RectangleEdge edges{RectangleEdge::All};
            RectangleFlatEdge flatEdges{RectangleFlatEdge::None};
            RectangleRoundedCorner roundedCorners{RectangleRoundedCorner::None};
            RectangleYap yaps{RectangleYap::None};
            Color borderColor{ColorFullBlack};
            Color fillColor{ColorNoColor};

            static auto from(const DrawRectangle &command) -> DrawableStyle;
        };

        static void drawFlat(Context *ctx, Point position, Length width, Length height, const DrawableStyle &style);

        static void draw(Context *ctx, Point position, Length width, Length height, const DrawableStyle &style);

      private:
        static void fillFlatRectangle(Context *ctx, Point position, Length width, Length height, Color color);
        static void fill(Context *ctx, Point startPosition, Color borderColor, Color fillColor);

        static void drawSides(Context *ctx,
                              Point position,
                              Length width,
                              Length height,
                              Length penWidth,
                              Color color,
                              RectangleEdge sides);
        static void drawSides(Context *ctx,
                              Point position,
                              Length width,
                              Length height,
                              Length radius,
                              Length yapSize,
                              Length penWidth,
                              RectangleFlatEdge flats,
                              RectangleYap yaps,
                              Color borderColor,
                              RectangleEdge sides);

        static void drawCorners(Context *ctx,
                                Point position,
                                Length width,
                                Length height,
                                Length radius,
                                Length penWidth,
                                Color borderColor,
                                RectangleRoundedCorner rounded,
                                RectangleFlatEdge flats,
                                RectangleYap yaps);

        static void drawTopSide(Context *ctx, Point position, Length width, Length penWidth, Color color);
        static void drawTopSide(Context *ctx,
                                Point position,
                                Length width,
                                Length radius,
                                Length yapSize,
                                Length penWidth,
                                RectangleFlatEdge flat,
                                RectangleYap yaps,
                                Color borderColor);

        static void drawRightSide(
            Context *ctx, Point position, Length width, Length height, Length penWidth, Color color);
        static void drawRightSide(Context *ctx,
                                  Point position,
                                  Length width,
                                  Length height,
                                  Length radius,
                                  Length yapSize,
                                  Length penWidth,
                                  RectangleFlatEdge flat,
                                  RectangleYap yaps,
                                  Color borderColor);

        static void drawBottomSide(
            Context *ctx, Point position, Length width, Length height, Length penWidth, Color color);
        static void drawBottomSide(Context *ctx,
                                   Point position,
                                   Length width,
                                   Length height,
                                   Length radius,
                                   Length yapSize,
                                   Length penWidth,
                                   RectangleFlatEdge flat,
                                   RectangleYap yaps,
                                   Color borderColor);

        static void drawLeftSide(Context *ctx, Point position, Length height, Length penWidth, Color color);
        static void drawLeftSide(Context *ctx,
                                 Point position,
                                 Length height,
                                 Length radius,
                                 Length yapSize,
                                 Length penWidth,
                                 RectangleFlatEdge flat,
                                 RectangleYap yaps,
                                 Color borderColor);
    };
} // namespace gui::renderer
