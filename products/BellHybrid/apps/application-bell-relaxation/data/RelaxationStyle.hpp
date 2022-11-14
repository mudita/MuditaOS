// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::relaxationStyle
{
    inline constexpr auto descriptionFont = style::window::font::verybiglight;
    inline constexpr auto titleFont       = style::window::font::largelight;
    inline constexpr auto clockFont       = style::window::font::largelight;
    inline constexpr auto timerValueFont  = style::window::font::supersizemelight;
    inline constexpr auto valumeValueFont = style::window::font::supersizemelight;

    namespace ended
    {
        static constexpr auto image_top_margin    = 170U;
        static constexpr auto image_bottom_margin = 30U;
    } // namespace ended

    namespace title
    {
        inline constexpr auto bottomDescTopMargin = 15U;
        inline constexpr auto maxProgressValue    = 16U;
        inline constexpr auto maxLines            = 2U;
        inline constexpr auto width               = 400U;
    } // namespace title

    namespace text
    {
        inline constexpr auto maxLines = title::maxLines;
        inline constexpr auto minWidth = 300U;
    } // namespace text

    namespace pause
    {
        inline constexpr auto textH = 200U;
    }

    namespace relStyle
    {
        namespace progress
        {
            constexpr inline auto radius                   = 192U;
            constexpr inline auto penWidth                 = 3U;
            constexpr inline auto verticalDeviationDegrees = 38U;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto marginTop   = 41U;
            constexpr inline auto marginRight = 32U;
            constexpr inline auto font        = style::window::font::supersizeme;
            constexpr inline auto maxSizeX    = 340U;
            constexpr inline auto maxSizeY    = 198U;
        } // namespace timer

        namespace pauseIcon
        {
            constexpr inline auto image     = "big_pause";
            constexpr inline auto marginTop = 39U;
            constexpr inline auto maxSizeX  = 203U;
            constexpr inline auto maxSizeY  = 203U;
        } // namespace pauseIcon

        namespace clock
        {
            constexpr inline auto marginTop = 17U;
            constexpr inline auto maxSizeX  = 340U;
            constexpr inline auto maxSizeY  = 84U;
        } // namespace clock
    }     // namespace relStyle

} // namespace gui::relaxationStyle
