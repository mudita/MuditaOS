// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Color.hpp"
#include "Common.hpp"

#include <memory>

namespace gui
{
    class Context;
} // namespace gui

namespace gui::renderer
{
    class PixelRenderer
    {

      public:
        PixelRenderer() = delete;

        static void draw(Context *ctx, Point point, Color color);
        static void updateColorScheme(const std::unique_ptr<ColorScheme> &scheme);
        [[nodiscard]] static auto getColor(const uint8_t intensity) -> uint8_t;
    };
} // namespace gui::renderer
