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
            inline constexpr auto radius                   = 192;
            inline constexpr auto penWidth                 = 3;
            inline constexpr auto verticalDeviationDegrees = 38;
        } // namespace progress

        namespace timer
        {
            inline constexpr auto marginTop = 41;
            inline constexpr auto font      = style::window::font::supersizeme;
            inline constexpr auto maxSizeX  = 340;
            inline constexpr auto maxSizeY  = 198;
        } // namespace timer

        namespace pauseIcon
        {
            inline constexpr auto image     = "big_pause";
            inline constexpr auto marginTop = 39;
            inline constexpr auto maxSizeX  = 203;
            inline constexpr auto maxSizeY  = 203;
        } // namespace pauseIcon

        namespace ringIcon
        {
            inline constexpr auto image     = "big_bell_ringing";
            inline constexpr auto marginTop = 39;
            inline constexpr auto maxSizeX  = 210;
            inline constexpr auto maxSizeY  = 203;
        } // namespace ringIcon

        namespace clock
        {
            inline constexpr auto marginTop = 17;
            inline constexpr auto maxSizeX  = 340;
            inline constexpr auto maxSizeY  = 84;
        } // namespace clock
    }
} // namespace gui::powerNapStyle
