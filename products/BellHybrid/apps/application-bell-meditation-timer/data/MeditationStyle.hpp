// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace app::meditationStyle
{
    namespace itStyle
    {
        namespace icon
        {
            inline constexpr auto w           = 128;
            inline constexpr auto h           = 128;
            constexpr inline auto x           = (style::window_width - w) / 2;
            constexpr inline auto y           = style::window_height / 2 - h;
            constexpr inline auto imageSource = "logo_no_text";
            constexpr inline auto imageTopMargin    = 112;
            constexpr inline auto imageBottomMargin = 30;
        } // namespace icon

        namespace text
        {
            constexpr inline auto textMarginTop = 40;
            constexpr inline auto w             = style::window_width;
            constexpr inline auto h             = style::window_height / 2 - textMarginTop;
            constexpr inline auto x             = 0;
            constexpr inline auto y             = style::window_height / 2 + textMarginTop;
            constexpr inline auto font          = style::window::font::verybiglight;
        } // namespace text
    }     // namespace itStyle

    namespace icStyle
    {
        namespace title
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 120;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = 0;
            constexpr inline auto font = style::window::font::largelight;
        } // namespace title

        namespace text
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = style::window_height;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = 0;
            constexpr inline auto font = style::window::font::supersizemelight;
        } // namespace text

        namespace minute
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 120;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = style::window_height - h;
            constexpr inline auto font = style::window::font::largelight;
        } // namespace minute

        namespace list
        {
            constexpr inline auto timeUnitSingular = "common_minute_lower";
            constexpr inline auto timeUnitPlural   = "common_minutes_lower";
        } // namespace list
    }     // namespace icStyle

    namespace mrStyle
    {
        namespace datetime
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 80;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = 24;
            constexpr inline auto font = style::window::font::verybiglight;
        } // namespace datetime

        namespace title
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 160;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = datetime::y + datetime::h;
            constexpr inline auto font = style::window::font::largelight;
        } // namespace title

        namespace progress
        {
            constexpr inline auto progressMarginTop  = 40;
            constexpr inline auto progressMarginLeft = 60;
            constexpr inline auto w                  = style::window_width - progressMarginLeft * 2;
            constexpr inline auto h                  = 24;
            constexpr inline auto x                  = progressMarginLeft;
            constexpr inline auto y                  = title::y + title::h + progressMarginTop;
            constexpr inline auto boxesCount         = 16;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto timerMarginBottom = 20;
            constexpr inline auto w                 = style::window_width;
            constexpr inline auto h                 = 60;
            constexpr inline auto x                 = 0;
            constexpr inline auto y                 = style::window_height - timerMarginBottom - h;
            constexpr inline auto font              = style::window::font::largelight;
        } // namespace timer
    }     // namespace mrStyle

    namespace mtStyle
    {
        namespace title
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 120;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = 0;
            constexpr inline auto font = style::window::font::largelight;
        } // namespace title

        namespace text
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = style::window_height;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = 0;
            constexpr inline auto font = style::window::font::supersizemelight;
        } // namespace text

        namespace minute
        {
            constexpr inline auto w    = style::window_width;
            constexpr inline auto h    = 120;
            constexpr inline auto x    = 0;
            constexpr inline auto y    = style::window_height - h;
            constexpr inline auto font = style::window::font::largelight;
        } // namespace minute

        namespace list
        {
            constexpr inline auto timeUnitSingular = "common_minute_lower";
            constexpr inline auto timeUnitPlural   = "common_minutes_lower";
        } // namespace list
    }     // namespace mtStyle

    namespace spStyle
    {
        namespace icon
        {
            constexpr inline auto imageSource = "big_pause_W_G";
        } // namespace icon
    }     // namespace spStyle
} // namespace meditationStyle
