// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::powerNapStyle
{
    inline constexpr auto napPeriodFont = style::window::font::supersizeme;

    namespace progressStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = 192;
            constexpr inline auto penWidth                 = 3;
            constexpr inline auto verticalDeviationDegrees = 38;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop   = 41;
            constexpr inline auto marginRight = 32;
            constexpr inline auto font        = style::window::font::supersizeme;
            constexpr inline auto maxSizeX    = 340;
            constexpr inline auto maxSizeY    = 198;
        } // namespace timer

        namespace pauseIcon
        {
            constexpr inline auto image     = "big_pause";
            constexpr inline auto marginTop = 39;
            constexpr inline auto maxSizeX  = 203;
            constexpr inline auto maxSizeY  = 203;
        } // namespace pauseIcon

        namespace ringIcon
        {
            constexpr inline auto image     = "big_bell_ringing";
            constexpr inline auto marginTop = 39;
            constexpr inline auto maxSizeX  = 210;
            constexpr inline auto maxSizeY  = 203;
        } // namespace ringIcon

        namespace clock
        {
            constexpr inline auto marginTop = 17;
            constexpr inline auto maxSizeX  = 340;
            constexpr inline auto maxSizeY  = 84;
        } // namespace clock
    }
} // namespace gui::powerNapStyle
