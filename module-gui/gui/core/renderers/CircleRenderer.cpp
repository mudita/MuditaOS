// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CircleRenderer.hpp"

#include "ArcRenderer.hpp"
#include "PixelRenderer.hpp"

namespace gui::renderer
{
    auto CircleRenderer::DrawableStyle::from(const DrawCircle &command) -> DrawableStyle
    {
        return DrawableStyle{command.width, command.borderColor, command.filled ? command.fillColor : ColorNoColor};
    }

    void CircleRenderer::draw(Context *ctx, Point center, Length radius, const DrawableStyle &style)
    {
        if (style.fillColor == ColorNoColor) {
            draw(ctx, center, radius, style.borderColor, style.penWidth);
        }
        else {
            draw(ctx, center, radius, style.borderColor, style.penWidth, style.fillColor);
        }
    }

    void CircleRenderer::draw(
        Context *ctx, Point center, Length radius, Color borderColor, Length borderWidth, Color fillColor)
    {
        // First, fill the desired area.
        const auto r  = static_cast<int>(radius);
        const auto rr = r * r;
        for (auto x = -r; x < r; ++x) {
            const auto height = static_cast<int>(std::sqrt(rr - (x * x)));
            for (auto y = -height; y < height; ++y) {
                PixelRenderer::draw(ctx, Point(center.x + x, center.y + y), fillColor);
            }
        }

        // Next, draw a border on top.
        draw(ctx, center, radius, borderColor, borderWidth);
    }

    void CircleRenderer::draw(Context *ctx, Point center, Length radius, Color color, Length width)
    {
        ArcRenderer::draw(ctx, center, radius, 0, trigonometry::FullAngle, ArcRenderer::DrawableStyle{width, color});
    }

} // namespace gui::renderer
