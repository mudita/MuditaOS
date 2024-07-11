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
            inline constexpr auto radius                   = 192U;
            inline constexpr auto penWidth                 = 3U;
            inline constexpr auto verticalDeviationDegrees = 38U;
        } // namespace progress

        namespace timer
        {
            inline constexpr auto marginTop = 19U;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 198U;
        } // namespace timer

        namespace pauseIcon
        {
            inline constexpr auto image     = "big_pause";
            inline constexpr auto maxSizeX  = 203U;
            inline constexpr auto maxSizeY  = 203U;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace pauseIcon

        namespace clock
        {
            inline constexpr auto marginTop = 17U;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 84U;
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
            inline constexpr auto radius                   = runningStyle::progress::radius;
            inline constexpr auto penWidth                 = 9U;
            inline constexpr auto verticalDeviationDegrees = runningStyle::progress::verticalDeviationDegrees;
        } // namespace progress

        namespace timer
        {
            inline constexpr auto marginTop = 4U;
            inline constexpr auto maxSizeX  = runningStyle::timer::maxSizeX;
            inline constexpr auto maxSizeY  = runningStyle::timer::maxSizeY;
        } // namespace timer

        namespace description
        {
            inline constexpr auto font      = style::window::font::large;
            inline constexpr auto marginTop = 63U;
            inline constexpr auto maxSizeX  = 380U;
            inline constexpr auto maxSizeY  = 102U;
        } // namespace description

        namespace bottomDescription
        {
            inline constexpr auto marginTop = runningStyle::bottomDescription::marginTop;
            inline constexpr auto maxSizeX  = runningStyle::bottomDescription::maxSizeX;
            inline constexpr auto maxSizeY  = runningStyle::bottomDescription::maxSizeY;
            inline constexpr auto font      = runningStyle::bottomDescription::font;
        } // namespace bottomDescription
    }     // namespace countdownStyle

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
} // namespace app::meditationStyle
