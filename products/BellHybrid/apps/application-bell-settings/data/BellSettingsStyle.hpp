// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>
#include <Style.hpp>

namespace gui
{
    namespace bell_settings_style
    {
        namespace settings_window
        {
            inline constexpr auto options_list_margin_x = 100U;
            inline constexpr auto options_list_margin_y = 50U;
            inline constexpr auto default_body_width    = 400U;
        } // namespace settings_window

        namespace time_fmt_set_list_item
        {
            inline constexpr auto font = style::window::font::supersizemelight;
        } // namespace time_fmt_set_list_item

        namespace top_text
        {
            inline constexpr auto font = "gt_pressura_light_46";
        }
    } // namespace bell_settings_style
} // namespace gui
