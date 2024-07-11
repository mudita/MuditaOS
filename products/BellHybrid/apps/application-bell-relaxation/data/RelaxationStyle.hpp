// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::relaxationStyle
{
    inline constexpr auto descriptionFont = style::window::font::verybiglight;
    inline constexpr auto titleFont       = style::window::font::large;
    inline constexpr auto timerValueFont  = style::window::font::supersizemelight;
    inline constexpr auto volumeValueFont = style::window::font::supersizemelight;

    namespace ended
    {
        inline constexpr auto imageMarginTop    = 170U;
        inline constexpr auto imageMarginBottom = 30U;
    } // namespace ended

    namespace title
    {
        inline constexpr auto loopMarginTop       = 42U;
        inline constexpr auto maxLines            = 2U;
        inline constexpr auto width               = 400U;
    } // namespace title

    namespace relStyle
    {
        namespace progressTime
        {
            inline constexpr auto radius                   = 192U;
            inline constexpr auto penWidth                 = 3U;
            inline constexpr auto verticalDeviationDegrees = 38U;
        } // namespace progressTime

        namespace progressVolume
        {
            inline constexpr auto radius                   = 192U;
            inline constexpr auto penWidth                 = 3U;
            inline constexpr auto verticalDeviationDegrees = 38U + 30U;
        } // namespace progressVolume

        namespace timer
        {
            inline constexpr auto marginTop = 19U;
            inline constexpr auto font      = style::window::font::supersizeme;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 198U;
        } // namespace timer

        namespace pauseIcon
        {
            inline constexpr auto image     = "big_pause";
            inline constexpr auto minSizeX        = 203U;
            inline constexpr auto loopMinSizeY    = 140U;
            inline constexpr auto runningMinSizeY = 154U;
            inline constexpr auto marginTop       = 63U;
        } // namespace pauseIcon

        namespace clock
        {
            inline constexpr auto marginTop = 17U;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 84U;
        } // namespace clock

        namespace loopedDescription
        {
            inline constexpr auto marginTop = -8;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 40U;
            inline constexpr auto font      = style::window::font::big;
        } // namespace loopedDescription

        namespace bottomDescription
        {
            inline constexpr auto loopMarginTop   = 15U;
            inline constexpr auto pausedMarginTop = 10U;
            inline constexpr auto maxSizeX  = 340U;
            inline constexpr auto maxSizeY  = 80U;
            inline constexpr auto font      = style::window::font::verybig;
        } // namespace bottomDescription

        namespace battery
        {
            inline constexpr auto loopMarginTop = 74U;
        }
    } // namespace relStyle
} // namespace gui::relaxationStyle
