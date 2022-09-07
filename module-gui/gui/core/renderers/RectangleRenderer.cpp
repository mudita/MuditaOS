// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RectangleRenderer.hpp"

#include "ArcRenderer.hpp"
#include "LineRenderer.hpp"
#include "PixelRenderer.hpp"

#include "Context.hpp"

#include <queue>

namespace gui::renderer
{
    auto RectangleRenderer::DrawableStyle::from(const DrawRectangle &command) -> DrawableStyle
    {
        return DrawableStyle{command.penWidth,
                             command.radius,
                             command.yapSize,
                             command.edges,
                             command.flatEdges,
                             command.corners,
                             command.yaps,
                             command.borderColor,
                             command.filled ? command.fillColor : ColorNoColor};
    }

    void RectangleRenderer::drawFlat(
        Context *ctx, Point position, Length width, Length height, const DrawableStyle &style)
    {
        if (style.fillColor != ColorNoColor) {
            fillFlatRectangle(ctx, position, width, height, style.fillColor);
        }
        drawSides(ctx, position, width, height, style.borderWidth, style.borderColor, style.edges);
    }

    void RectangleRenderer::fillFlatRectangle(Context *ctx, Point position, Length width, Length height, Color color)
    {
        for (unsigned int y = 1; y <= height; ++y) {
            LineRenderer::drawHorizontal(ctx,
                                         Point(position.x, position.y + y),
                                         width,
                                         LineRenderer::DrawableStyle{1, color, LineExpansionDirection::Up});
        }
    }

    void RectangleRenderer::drawSides(
        Context *ctx, Point position, Length width, Length height, Length penWidth, Color color, RectangleEdge sides)
    {
        if (sides & RectangleEdge::Top) {
            drawTopSide(ctx, position, width, penWidth, color);
        }
        if (sides & RectangleEdge::Right) {
            drawRightSide(ctx, position, width, height, penWidth, color);
        }
        if (sides & RectangleEdge::Bottom) {
            drawBottomSide(ctx, position, width, height, penWidth, color);
        }
        if (sides & RectangleEdge::Left) {
            drawLeftSide(ctx, position, height, penWidth, color);
        }
    }

    void RectangleRenderer::draw(Context *ctx, Point position, Length width, Length height, const DrawableStyle &style)
    {
        drawCorners(ctx,
                    position,
                    width,
                    height,
                    style.radius,
                    style.borderWidth,
                    style.borderColor,
                    style.roundedCorners,
                    style.flatEdges,
                    style.yaps);
        drawSides(ctx,
                  position,
                  width,
                  height,
                  style.radius,
                  style.yapSize,
                  style.borderWidth,
                  style.flatEdges,
                  style.yaps,
                  style.borderColor,
                  style.edges);
        if (style.fillColor != ColorNoColor) {
            fill(ctx, Point(position.x + (width / 2), position.y + (height / 2)), style.borderColor, style.fillColor);
        }
    }

    void RectangleRenderer::drawCorners(Context *ctx,
                                        Point position,
                                        Length width,
                                        Length height,
                                        Length radius,
                                        Length penWidth,
                                        Color borderColor,
                                        RectangleRoundedCorner rounded,
                                        RectangleFlatEdge flats,
                                        RectangleYap yaps)
    {

        const ArcRenderer::DrawableStyle arcStyle{penWidth, borderColor};

        const auto topRightCenter    = Point(position.x + width - radius, position.y + radius);
        const auto topLeftCenter     = Point(position.x + radius, topRightCenter.y);
        const auto bottomRightCenter = Point(topRightCenter.x, position.y + height - radius);
        const auto bottomLeftCenter  = Point(topLeftCenter.x, bottomRightCenter.y);

        const auto topRightCornerRounded = (rounded & RectangleRoundedCorner::TopRight) &&
                                           !(flats & RectangleFlatEdge::TopRight) && !(yaps & RectangleYap::TopRight);
        if (topRightCornerRounded) {
            ArcRenderer::draw(ctx, topRightCenter, radius, -90, 90, arcStyle);
        }

        const auto topLeftCornerRounded = (rounded & RectangleRoundedCorner::TopLeft) &&
                                          !(flats & RectangleFlatEdge::TopLeft) && !(yaps & RectangleYap::TopLeft);
        if (topLeftCornerRounded) {
            ArcRenderer::draw(ctx, topLeftCenter, radius, -180, 90, arcStyle);
        }

        const auto bottomRightCornerRounded = (rounded & RectangleRoundedCorner::BottomRight) &&
                                              !(flats & RectangleFlatEdge::BottomRight) &&
                                              !(yaps & RectangleYap::BottomRight);
        if (bottomRightCornerRounded) {
            ArcRenderer::draw(ctx, bottomRightCenter, radius, 0, 90, arcStyle);
        }

        const auto bottomLeftCornerRounded = (rounded & RectangleRoundedCorner::BottomLeft) &&
                                             !(flats & RectangleFlatEdge::BottomLeft) &&
                                             !(yaps & RectangleYap::BottomLeft);
        if (bottomLeftCornerRounded) {
            ArcRenderer::draw(ctx, bottomLeftCenter, radius, -270, 90, arcStyle);
        }
    }

    void RectangleRenderer::drawSides(Context *ctx,
                                      Point position,
                                      Length width,
                                      Length height,
                                      Length radius,
                                      Length yapSize,
                                      Length penWidth,
                                      RectangleFlatEdge flats,
                                      RectangleYap yaps,
                                      Color borderColor,
                                      RectangleEdge sides)
    {
        if (sides & RectangleEdge::Top) {
            drawTopSide(ctx, position, width, radius, yapSize, penWidth, flats, yaps, borderColor);
        }
        if (sides & RectangleEdge::Bottom) {
            drawBottomSide(ctx, position, width, height, radius, yapSize, penWidth, flats, yaps, borderColor);
        }
        if (sides & RectangleEdge::Left) {
            drawLeftSide(ctx, position, height, radius, yapSize, penWidth, flats, yaps, borderColor);
        }
        if (sides & RectangleEdge::Right) {
            drawRightSide(ctx, position, width, height, radius, yapSize, penWidth, flats, yaps, borderColor);
        }
    }

    void RectangleRenderer::fill(Context *ctx, Point startPosition, Color borderColor, Color fillColor)
    {
        std::queue<Point> q;
        q.push(startPosition);

        while (!q.empty()) {
            const auto currPoint = q.front();
            q.pop();
            if (const auto color = ctx->getPixel(currPoint, PixelRenderer::getColor(borderColor.intensity));
                color == PixelRenderer::getColor(borderColor.intensity) ||
                color == PixelRenderer::getColor(fillColor.intensity)) {
                continue;
            }

            PixelRenderer::draw(ctx, currPoint, fillColor);
            q.push(Point{currPoint.x + 1, currPoint.y});
            q.push(Point{currPoint.x - 1, currPoint.y});
            q.push(Point{currPoint.x, currPoint.y + 1});
            q.push(Point{currPoint.x, currPoint.y - 1});
        }
    }

    void RectangleRenderer::drawTopSide(Context *ctx, Point position, Length length, Length penWidth, Color color)
    {
        LineRenderer::drawHorizontal(
            ctx, position, length, LineRenderer::DrawableStyle{penWidth, color, LineExpansionDirection::Down});
    }

    void RectangleRenderer::drawTopSide(Context *ctx,
                                        Point position,
                                        Length width,
                                        Length radius,
                                        Length yapSize,
                                        Length penWidth,
                                        RectangleFlatEdge flat,
                                        RectangleYap yaps,
                                        Color borderColor)
    {
        int x = position.x;
        if (yaps & RectangleYap::TopLeft) {
            x -= yapSize;
            width += yapSize;
        }
        else if (!(flat & RectangleFlatEdge::TopLeft)) {
            x += radius;
            width -= radius;
        }

        if (yaps & RectangleYap::TopRight) {
            width += yapSize;
        }
        else if (!(flat & RectangleFlatEdge::TopRight)) {
            width -= radius;
        }
        LineRenderer::drawHorizontal(ctx,
                                     Point(x, position.y),
                                     width,
                                     LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Down});
    }

    void RectangleRenderer::drawRightSide(
        Context *ctx, Point position, Length width, Length height, Length penWidth, Color color)
    {
        LineRenderer::drawVertical(ctx,
                                   Point(position.x + width, position.y),
                                   height,
                                   LineRenderer::DrawableStyle{penWidth, color, LineExpansionDirection::Left});
    }

    void RectangleRenderer::drawRightSide(Context *ctx,
                                          Point position,
                                          Length width,
                                          Length height,
                                          Length radius,
                                          Length yapSize,
                                          Length penWidth,
                                          RectangleFlatEdge flat,
                                          RectangleYap yaps,
                                          Color borderColor)
    {
        int x      = position.x + width + 1;
        int y      = position.y;
        int length = height;
        if (yaps & RectangleYap::TopRight) {
            LineRenderer::draw45deg(ctx,
                                    Point(x + yapSize - 1, y),
                                    yapSize,
                                    LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Left},
                                    false);
            y += yapSize;
            length -= yapSize;
        }
        else if (!(flat & RectangleFlatEdge::TopRight)) {
            y += radius;
            length -= radius;
        }

        if (yaps & RectangleYap::BottomRight) {
            LineRenderer::draw45deg(ctx,
                                    Point(x, y + height),
                                    yapSize,
                                    LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Left},
                                    true);
            length -= yapSize;
        }
        else if (!(flat & RectangleFlatEdge::BottomRight)) {
            length -= radius;
        }

        LineRenderer::drawVertical(
            ctx, Point(x, y), length, LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Left});
    }

    void RectangleRenderer::drawBottomSide(
        Context *ctx, Point position, Length width, Length height, Length penWidth, Color color)
    {
        LineRenderer::drawHorizontal(ctx,
                                     Point(position.x, position.y + height),
                                     width,
                                     LineRenderer::DrawableStyle{penWidth, color, LineExpansionDirection::Up});
    }

    void RectangleRenderer::drawBottomSide(Context *ctx,
                                           Point position,
                                           Length width,
                                           Length height,
                                           Length radius,
                                           Length yapSize,
                                           Length penWidth,
                                           RectangleFlatEdge flat,
                                           RectangleYap yaps,
                                           Color borderColor)
    {
        int x      = position.x;
        int y      = position.y + height + 1;
        int length = width;
        if (yaps & RectangleYap::BottomLeft) {
            x -= yapSize;
            length += yapSize;
        }
        else if (!(flat & RectangleFlatEdge::BottomLeft)) {
            x += radius;
            length -= radius;
        }

        if (yaps & RectangleYap::BottomRight) {
            length += yapSize;
        }
        else if (!(flat & RectangleFlatEdge::BottomRight)) {
            length -= radius;
        }
        LineRenderer::drawHorizontal(
            ctx, Point(x, y), length, LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Up});
    }

    void RectangleRenderer::drawLeftSide(Context *ctx, Point position, Length height, Length penWidth, Color color)
    {
        LineRenderer::drawVertical(
            ctx, position, height, LineRenderer::DrawableStyle{penWidth, color, LineExpansionDirection::Right});
    }

    void RectangleRenderer::drawLeftSide(Context *ctx,
                                         Point position,
                                         Length height,
                                         Length radius,
                                         Length yapSize,
                                         Length penWidth,
                                         RectangleFlatEdge flat,
                                         RectangleYap yaps,
                                         Color borderColor)
    {
        int x      = position.x;
        int y      = position.y;
        int length = height;
        if (yaps & RectangleYap::TopLeft) {
            LineRenderer::draw45deg(ctx,
                                    Point(x - yapSize, y),
                                    yapSize,
                                    LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Right},
                                    true);
            y += yapSize;
            length -= yapSize;
        }
        else if (!(flat & RectangleFlatEdge::TopLeft)) {
            y += radius;
            length -= radius;
        }

        if (yaps & RectangleYap::BottomLeft) {
            LineRenderer::draw45deg(ctx,
                                    Point(x, y + height),
                                    yapSize,
                                    LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Right},
                                    false);
            length -= yapSize;
        }
        else if (!(flat & RectangleFlatEdge::BottomLeft)) {
            length -= radius;
        }
        LineRenderer::drawVertical(ctx,
                                   Point(x, y),
                                   length,
                                   LineRenderer::DrawableStyle{penWidth, borderColor, LineExpansionDirection::Right});
    }
} // namespace gui::renderer
