// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PixelRenderer.hpp"
#include "Context.hpp"

#include <cstring>

namespace gui::renderer
{
    static ColorScheme colorScheme = ::gui::Color::defaultColorScheme;

    void PixelRenderer::draw(Context *ctx, Point point, Color color)
    {
        const auto contextWidth = ctx->getW();
        const auto position     = point.y * contextWidth + point.x;

        std::memset(ctx->getData() + position, colorScheme.intensity[color.intensity], 1);
    }

    void PixelRenderer::updateColorScheme(const std::unique_ptr<ColorScheme> &scheme)
    {
        colorScheme = *scheme;
    }

    auto PixelRenderer::getColor(const uint8_t intensity) -> uint8_t
    {
        return colorScheme.intensity[intensity];
    }
} // namespace gui::renderer
