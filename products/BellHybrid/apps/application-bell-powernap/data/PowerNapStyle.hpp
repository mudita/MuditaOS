// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
            inline constexpr auto radius                   = 192U;
            inline constexpr auto penWidth                 = 3U;
            inline constexpr auto verticalDeviationDegrees = 38U;
        } // namespace progress

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
            inline constexpr auto maxSizeX  = 203U;
            inline constexpr auto maxSizeY  = 203U;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace pauseIcon

        namespace ringIcon
        {
            inline constexpr auto image     = "big_bell_ringing";
            inline constexpr auto maxSizeX  = 210U;
            inline constexpr auto maxSizeY  = 203U;
            inline constexpr auto marginTop = timer::marginTop - (maxSizeY - timer::maxSizeY);
        } // namespace ringIcon

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
    }
} // namespace gui::powerNapStyle
