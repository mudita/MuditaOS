// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LineRenderer.hpp"
#include "PixelRenderer.hpp"

#include "Context.hpp"

#include <cmath>

namespace gui::renderer
{
    namespace
    {
        constexpr auto toSymmetricPenWidth(std::uint16_t penWidth) noexcept -> std::uint16_t
        {
            switch (penWidth) {
            case 1:
                return 1;
            case 2:
                return 3;
            case 3:
                return 5;
            default:
                return penWidth * M_SQRT2;
            }
        }
    } // namespace

    auto LineRenderer::DrawableStyle::from(const DrawLine &command) -> DrawableStyle
    {
        DrawableStyle details;
        details.penWidth = command.penWidth;
        details.color    = command.color;
        return details;
    }

    void LineRenderer::draw(Context *ctx, Point start, Point end, Color color)
    {
        if (color.alpha == Color::FullTransparent) {
            return;
        }

        const int distanceX = std::abs(end.x - start.x);
        const int distanceY = std::abs(end.y - start.y);
        const auto step     = distanceX >= distanceY ? distanceX : distanceY;

        auto dx = static_cast<float>(distanceX) / step;
        dx      = end.x < start.x ? -dx : dx;
        auto dy = static_cast<float>(distanceY) / step;
        dy      = end.y < start.y ? -dy : dy;

        float x = start.x;
        float y = start.y;
        for (int i = 0; i < step; ++i) {
            PixelRenderer::draw(ctx, Point(x, y), color);
            x += dx;
            y += dy;
        }
    }

    void LineRenderer::drawHorizontal(Context *ctx, Point start, Length width, const DrawableStyle &style)
    {
        if (style.color.alpha == Color::FullTransparent) {
            return;
        }
        if (style.direction != LineExpansionDirection::Down && style.direction != LineExpansionDirection::Up) {
            return;
        }

        for (Length i = 0; i < style.penWidth; ++i) {
            const auto offset = (style.direction == LineExpansionDirection::Down) ? i : -i - 1;
            draw(ctx, Point(start.x, start.y + offset), Point(start.x + width, start.y + offset), style.color);
        }
    }

    void LineRenderer::drawVertical(Context *ctx, Point start, Length height, const DrawableStyle &style)
    {
        if (style.color.alpha == Color::FullTransparent) {
            return;
        }
        if (style.direction != LineExpansionDirection::Left && style.direction != LineExpansionDirection::Right) {
            return;
        }

        for (Length i = 0; i < style.penWidth; ++i) {
            const auto offset = (style.direction == LineExpansionDirection::Right) ? i : -i - 1;
            draw(ctx, Point(start.x + offset, start.y), Point(start.x + offset, start.y + height), style.color);
        }
    }

    void LineRenderer::draw45deg(Context *ctx, Point start, Length length, const DrawableStyle &style, bool toRight)
    {
        // if color is fully transparent - return
        if (style.color.alpha == Color::FullTransparent) {
            return;
        }

        const auto end =
            toRight ? Point(start.x + length, start.y + length) : Point(start.x - length, start.y + length);
        drawSlanting(ctx, start, end, toSymmetricPenWidth(style.penWidth), style.color, style.direction);
    }

    void LineRenderer::drawSlanting(
        Context *ctx, Point start, Point end, Length penWidth, Color color, LineExpansionDirection expansionDirection)
    {
        if (color.alpha == Color::FullTransparent) {
            return;
        }
        if (expansionDirection != LineExpansionDirection::Left && expansionDirection != LineExpansionDirection::Right) {
            return;
        }

        const int directionFactor = expansionDirection == LineExpansionDirection::Left ? -1 : 1;
        for (Length row = 0; row < penWidth; ++row) {
            draw(ctx,
                 Point(start.x + (directionFactor * row), start.y),
                 Point(end.x + (directionFactor * row), end.y),
                 color);
        }
    }
} // namespace gui::renderer
