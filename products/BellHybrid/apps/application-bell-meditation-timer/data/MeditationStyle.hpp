// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace app::meditationStyle
{
    namespace itStyle
    {
        namespace icon
        {
            constexpr inline auto imageLogo         = "bell_big_logo";
            constexpr inline auto imagePause        = "big_pause";
            constexpr inline auto imageTopMargin    = 112;
            constexpr inline auto imageBottomMargin = 30;
        } // namespace icon

        namespace text
        {
            constexpr inline auto font = style::window::font::verybiglight;
        } // namespace text
    }     // namespace itStyle

    namespace icStyle
    {
        namespace text
        {
            constexpr inline auto font = style::window::font::supersizemelight;
        } // namespace text

        namespace minute
        {
            constexpr inline auto font = style::window::font::largelight;
        } // namespace minute

    } // namespace icStyle

    namespace mrStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = 192;
            constexpr inline auto penWidth                 = 3;
            constexpr inline auto verticalDeviationDegrees = 38;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop = 39;
            constexpr inline auto font      = style::window::font::supersizeme;
            constexpr inline auto maxSizeX  = 340;
            constexpr inline auto maxSizeY  = 198;
        } // namespace timer

        namespace clock
        {
            constexpr inline auto marginTop = 17;
            constexpr inline auto maxSizeX  = 340;
            constexpr inline auto maxSizeY  = 84;
        } // namespace clock
    }     // namespace mrStyle

    namespace mtStyle
    {
        namespace text
        {
            constexpr inline auto font = style::window::font::supersizemelight;
        } // namespace text

        namespace minute
        {
            constexpr inline auto font = style::window::font::largelight;
        } // namespace minute

        namespace list
        {
            constexpr inline auto timeUnitSingular = "common_minute_lower";
            constexpr inline auto timeUnitPlural   = "common_minutes_lower";
            constexpr inline auto timeUnitGenitive = "common_minutes_lower_genitive";
        } // namespace list
    }     // namespace mtStyle
} // namespace app::meditationStyle
