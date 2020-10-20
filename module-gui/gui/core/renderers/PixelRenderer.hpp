// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Color.hpp"
#include "Common.hpp"

namespace gui
{
    class Context;
} // namespace gui

namespace gui::renderer
{
    class PixelRenderer
    {
      public:
        static void draw(Context *ctx, Point point, Color color);
    };
} // namespace gui::renderer
