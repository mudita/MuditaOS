// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ArcRenderer.hpp"
#include "PixelRenderer.hpp"

namespace gui::renderer
{
    constexpr Length RadiusPrecisionLimit = 5;

    auto ArcRenderer::DrawableStyle::from(const CommandArc &command) -> DrawableStyle
    {
        return DrawableStyle{command.width, command.borderColor};
    }

    void ArcRenderer::draw(Context *ctx,
                           Point center,
                           Length radius,
                           trigonometry::Degrees begin,
                           trigonometry::Degrees sweep,
                           const DrawableStyle &style)
    {
        if (style.penWidth == 1) {
            draw(ctx, center, radius, begin, sweep, style.color);
        }
        else {
            draw(ctx, center, radius, begin, sweep, style.color, style.penWidth);
        }
    }

    void ArcRenderer::draw(Context *ctx,
                           Point center,
                           Length radius,
                           trigonometry::Degrees begin,
                           trigonometry::Degrees sweep,
                           Color color,
                           Length width)
    {
        if (width == 1) {
            draw(ctx, center, radius, begin, sweep, color);
            return;
        }

        const auto start = trigonometry::toRadians(begin);
        const auto end   = trigonometry::toRadians(begin + sweep);
        double step;

        if (radius < RadiusPrecisionLimit) {
            step = 0.01;
        }
        else {
            step = 0.001;
        }

        double cosine, sine;
        for (double radians = start; radians <= end; radians += step) {
            cosine = std::cos(radians);
            sine   = std::sin(radians);
            for (Length i = 0; i < width; ++i) {
                const auto r = radius - i;
                const auto x = trigonometry::AdjacentSide::fromCosine(cosine, r);
                const auto y = trigonometry::OppositeSide::fromSine(sine, r);
                PixelRenderer::draw(ctx, Point(center.x + x, center.y + y), color);
            }
        }
    }

    void ArcRenderer::draw(Context *ctx,
                           Point center,
                           Length radius,
                           trigonometry::Degrees begin,
                           trigonometry::Degrees sweep,
                           Color color)
    {
        const auto start = trigonometry::toRadians(begin);
        const auto end   = trigonometry::toRadians(begin + sweep);
        double step;

        if (radius < RadiusPrecisionLimit) {
            step = 0.01;
        }
        else {
            step = 0.001;
        }

        long int x, y;
        for (double radians = start; radians <= end; radians += step) {
            x = trigonometry::AdjacentSide::fromAngle(radians, radius);
            y = trigonometry::OppositeSide::fromAngle(radians, radius);
            PixelRenderer::draw(ctx, Point(center.x + x, center.y + y), color);
        }
    }
} // namespace gui::renderer
