// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace app::meditationStyle
{
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

        namespace pauseIcon
        {
            constexpr inline auto image     = "big_pause";
            constexpr inline auto marginTop = 39;
            constexpr inline auto maxSizeX  = 203;
            constexpr inline auto maxSizeY  = 203;
        } // namespace pauseIcon

        namespace clock
        {
            constexpr inline auto marginTop = 17;
            constexpr inline auto maxSizeX  = 340;
            constexpr inline auto maxSizeY  = 84;
        } // namespace clock
    }     // namespace mrStyle

    namespace mcStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = mrStyle::progress::radius;
            constexpr inline auto penWidth                 = mrStyle::progress::penWidth;
            constexpr inline auto verticalDeviationDegrees = mrStyle::progress::verticalDeviationDegrees;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop = 23;
            constexpr inline auto font      = mrStyle::timer::font;
            constexpr inline auto maxSizeX  = mrStyle::timer::maxSizeX;
            constexpr inline auto maxSizeY  = mrStyle::timer::maxSizeY;
        } // namespace timer

        namespace description
        {
            constexpr inline auto font      = style::window::font::large;
            constexpr inline auto marginTop = 63;
            constexpr inline auto maxSizeX  = 380;
            constexpr inline auto maxSizeY  = 102;
        } // namespace description
    }     // namespace mcStyle

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
