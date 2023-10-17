// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>

#include <optional>

namespace service::gui
{
    struct RenderReference
    {
        int contextId;
        ::gui::RefreshModes refreshMode;
    };

    class RenderCache
    {
      public:
        std::optional<RenderReference> getCachedRender() const;
        bool isRenderCached() const noexcept;
        void cache(RenderReference render);
        void invalidate();

      private:
        void exchange(RenderReference render);

        std::optional<RenderReference> cachedRender;
    };
} // namespace service::gui
