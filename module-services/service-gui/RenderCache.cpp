// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RenderCache.hpp"

namespace service::gui
{
    std::optional<RenderReference> RenderCache::getCachedRender() const
    {
        return cachedRender;
    }

    bool RenderCache::isRenderCached() const noexcept
    {
        return cachedRender.has_value();
    }

    void RenderCache::cache(RenderReference render)
    {
        if (isRenderCached()) {
            exchange(render);
        }
        else {
            cachedRender = render;
        }
    }

    void RenderCache::exchange(RenderReference render)
    {
        if (cachedRender->refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
            render.refreshMode = cachedRender->refreshMode;
        }
        cachedRender = render;
    }

    void RenderCache::invalidate()
    {
        cachedRender = std::nullopt;
    }
} // namespace service::gui
