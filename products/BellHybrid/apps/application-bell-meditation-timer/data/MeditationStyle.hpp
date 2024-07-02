// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace app::meditationStyle
{
    namespace runningStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = 192U;
            constexpr inline auto penWidth                 = 3U;
            constexpr inline auto verticalDeviationDegrees = 38U;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop = 19U;
            constexpr inline auto maxSizeX  = 340U;
            constexpr inline auto maxSizeY  = 198U;
        } // namespace timer

        namespace pauseIcon
        {
            constexpr inline auto image     = "big_pause";
            constexpr inline auto maxSizeX  = 203U;
            constexpr inline auto maxSizeY  = 203U;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace pauseIcon

        namespace clock
        {
            constexpr inline auto marginTop = 17U;
            constexpr inline auto maxSizeX  = 340U;
            constexpr inline auto maxSizeY  = 84U;
        } // namespace clock

        namespace bottomDescription
        {
            inline constexpr auto marginTop = 38U;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 80U;
            inline constexpr auto font      = style::window::font::verybig;
        } // namespace bottomDescription
    }     // namespace runningStyle

    namespace countdownStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = runningStyle::progress::radius;
            constexpr inline auto penWidth                 = 9U;
            constexpr inline auto verticalDeviationDegrees = runningStyle::progress::verticalDeviationDegrees;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop = 24U;
            constexpr inline auto maxSizeX  = runningStyle::timer::maxSizeX;
            constexpr inline auto maxSizeY  = runningStyle::timer::maxSizeY;
        } // namespace timer

        namespace description
        {
            constexpr inline auto font      = style::window::font::large;
            constexpr inline auto marginTop = 63U;
            constexpr inline auto maxSizeX  = 380U;
            constexpr inline auto maxSizeY  = 102U;
        } // namespace description
    }     // namespace countdownStyle

    namespace timerStyle
    {
        namespace text
        {
            constexpr inline auto font = style::window::font::supersizemelight;
        }

        namespace minute
        {
            constexpr inline auto font = style::window::font::largelight;
        }
    } // namespace timerStyle
} // namespace app::meditationStyle
