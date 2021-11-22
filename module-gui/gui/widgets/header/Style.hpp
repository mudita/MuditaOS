// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::header
{
    namespace title
    {
        inline constexpr auto top_padding = 6u;
        inline constexpr auto margins     = 30u;
    }; // namespace title

    namespace navigation_indicator
    {
        inline constexpr auto h                 = 32u;
        inline constexpr auto w                 = 60u;
        inline constexpr auto ice_w             = 40u;
        inline constexpr auto top_margin        = 7u;
        inline constexpr auto right_left_margin = 22u;

        inline constexpr auto box_width = w + right_left_margin;
    } // namespace navigation_indicator

}; // namespace style::header
