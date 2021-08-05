// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ThreeBox.hpp>

namespace style::bell_base_layout
{
    constexpr inline auto w                  = style::window_width;
    constexpr inline auto h                  = style::window_height;
    constexpr inline auto first_layout_min_h = 50U;
    constexpr inline auto outer_layouts_h    = 120U;
} // namespace style::bell_base_layout

namespace gui
{
    class BellBaseLayout : public VThreeBox<VBox, VBox, VBox>
    {
      public:
        explicit BellBaseLayout(Item *parent, Position x = 0, Position y = 0, Length w = 0, Length h = 0);
    };

} /* namespace gui */
