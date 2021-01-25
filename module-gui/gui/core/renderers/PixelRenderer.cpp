// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PixelRenderer.hpp"
#include "Context.hpp"

#include <cstring>

namespace gui::renderer
{
    void PixelRenderer::draw(Context *ctx, Point point, Color color)
    {
        const auto contextWidth = ctx->getW();
        const auto position     = point.y * contextWidth + point.x;
        std::memset(ctx->getData() + position, color.intensity, 1);
    }
} // namespace gui::renderer
