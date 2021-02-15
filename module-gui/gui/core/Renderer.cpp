// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// for memset
#include "Renderer.hpp"
#include "Context.hpp"

// renderer
#include "renderers/PixelRenderer.hpp"

namespace gui
{
    void Renderer::changeColorScheme(const std::unique_ptr<ColorScheme> &scheme)
    {
        renderer::PixelRenderer::updateColorScheme(scheme);
    }

    void Renderer::render(Context *ctx, std::list<std::unique_ptr<DrawCommand>> &commands)
    {
        if (ctx == nullptr) {
            return;
        }

        for (auto &cmd : commands) {
            if (cmd == nullptr) {
                continue;
            }

            cmd->draw(ctx);
        }
    }

} /* namespace gui */
