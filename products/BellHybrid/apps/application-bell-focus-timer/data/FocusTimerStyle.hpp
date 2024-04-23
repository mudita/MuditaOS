// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>
#include "widgets/BellBaseLayout.hpp"

namespace app::focusTimerStyle
{
    namespace runningStyle
    {
        namespace progress
        {
            inline constexpr auto radius                   = 192u;
            inline constexpr auto penWidth                 = 3u;
            inline constexpr auto verticalDeviationDegrees = 38u;
        } // namespace progress

        namespace timer
        {
            inline constexpr auto font      = style::window::font::supersizeme;
            inline constexpr auto maxSizeX  = 340u;
            inline constexpr auto maxSizeY  = 198u;
            inline constexpr auto marginTop = 19u;
        } // namespace timer

        namespace pauseIcon
        {
            inline constexpr auto image     = "big_pause";
            inline constexpr auto maxSizeX  = 203u;
            inline constexpr auto maxSizeY  = 203u;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace pauseIcon

        namespace ringIcon
        {
            inline constexpr auto image     = "big_bell_ringing";
            inline constexpr auto maxSizeX  = 220u;
            inline constexpr auto maxSizeY  = 203u;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace ringIcon

        namespace bottomDescription
        {
            inline constexpr auto marginTop = 38u;
            inline constexpr auto maxSizeX  = 340u;
            inline constexpr auto maxSizeY  = 80u;
            inline constexpr auto font      = style::window::font::verybig;
        } // namespace bottomDescription

        namespace clock
        {
            inline constexpr auto marginTop = 17u;
            inline constexpr auto maxSizeX  = 340u;
            inline constexpr auto maxSizeY  = 84u;
        } // namespace clock
    }     // namespace runningStyle

    namespace timerStyle
    {
        namespace text
        {
            inline constexpr auto font = style::window::font::supersizemelight;
        }

        namespace minute
        {
            inline constexpr auto font = style::window::font::largelight;
        }
    } // namespace timerStyle
} // namespace app::focusTimerStyle
