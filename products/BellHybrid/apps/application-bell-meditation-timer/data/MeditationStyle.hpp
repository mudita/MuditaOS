// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace meditationStyle
{
    namespace itStyle
    {
        namespace icon
        {
            constexpr inline auto x           = style::window_width / 2 - 60;
            constexpr inline auto y           = style::header::height + 72;
            constexpr inline auto imageSource = "logo_no_text";
        } // namespace icon

        namespace text
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 60;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = style::window::default_body_height - 60;
        } // namespace text
    }     // namespace itStyle

    namespace icStyle
    {
        namespace left_arrow
        {
            constexpr inline auto x           = style::window::default_left_margin;
            constexpr inline auto y           = style::header::height + style::window::default_body_height / 2 - 6;
            constexpr inline auto imageSource = "arrow_left";
        } // namespace left_arrow

        namespace right_arrow
        {
            constexpr inline auto x           = style::window_width - style::window::default_right_margin - 6;
            constexpr inline auto y           = style::header::height + style::window::default_body_height / 2 - 6;
            constexpr inline auto imageSource = "arrow_right";
        } // namespace right_arrow

        namespace title
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 100;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 36;
        } // namespace title

        namespace text
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = style::window::default_body_height;
        } // namespace text

        namespace minute
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + style::window::default_body_height - 60;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 60;
        } // namespace minute
    }     // namespace icStyle

    namespace mrStyle
    {
        namespace datetime
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 24;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 36;
        } // namespace datetime

        namespace title
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 120;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 36;
        } // namespace title

        namespace progress
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 280;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 24;

            constexpr inline auto Radius               = 100;
            constexpr inline auto BorderColor          = gui::ColorGrey;
            constexpr inline auto BorderColorOnFocused = gui::ColorFullBlack;
            constexpr inline auto PenWidth             = 2;
        } // namespace progress

        namespace timer
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 320;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 36;
        } // namespace timer
    }     // namespace mrStyle

    namespace mtStyle
    {
        namespace title
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 20;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 36;
        } // namespace title

        namespace text
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = style::window::default_body_height;
        } // namespace text

        namespace minute
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + style::window::default_body_height - 60;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = 60;
        } // namespace minute
    }     // namespace mtStyle

    namespace spStyle
    {
        namespace icon
        {
            constexpr inline auto x           = style::window_width / 2 - 14;
            constexpr inline auto y           = style::header::height + 72;
            constexpr inline auto imageSource = "pause";
        } // namespace icon

        namespace text
        {
            constexpr inline auto x = 60;
            constexpr inline auto y = style::header::height + 60;
            constexpr inline auto w = style::window_width - 120;
            constexpr inline auto h = style::window::default_body_height - 60;
        } // namespace text
    }     // namespace spStyle
} // namespace meditationStyle
