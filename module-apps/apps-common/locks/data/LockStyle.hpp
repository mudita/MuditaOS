// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "gui/widgets/Style.hpp"

namespace style::window::lock_input
{
    namespace image
    {
        constexpr inline auto wh                  = 128;
        constexpr inline auto image_top_margin    = 30;
        constexpr inline auto image_bottom_margin = 33;
    } // namespace image

    namespace input_box
    {
        constexpr inline auto w            = style::window::default_body_width;
        constexpr inline auto h            = 60;
        constexpr inline auto label_margin = 10;
        constexpr inline auto top_margin   = 30;
    } // namespace input_box

    namespace primary_text
    {
        constexpr inline auto h = 60;
    } // namespace primary_text

    namespace secondary_text
    {
        constexpr inline auto top_margin = 30;
        constexpr inline auto h          = 90;
    } // namespace secondary_text

} // namespace style::window::lock_input
