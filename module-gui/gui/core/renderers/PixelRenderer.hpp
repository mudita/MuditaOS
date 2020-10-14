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
