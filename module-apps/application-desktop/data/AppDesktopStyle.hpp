// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "gui/widgets/Style.hpp"

namespace style::window::pin_lock
{
    namespace image
    {
        constexpr inline auto x = 177;
        constexpr inline auto y = 132;
    } // namespace image

    namespace pin_label
    {
        constexpr inline auto x = 85;
        constexpr inline auto y = 400;
        constexpr inline auto w = style::window_width - 2 * x;

        constexpr inline auto size   = 60;
        constexpr inline auto margin = 10;
    } // namespace pin_label

    namespace title
    {
        constexpr inline auto x = 0;
        constexpr inline auto y = 60;
        constexpr inline auto w = style::window_width;
        constexpr inline auto h = 50;
    } // namespace title

    namespace ice
    {
        constexpr inline auto x = style::window::default_left_margin;
        constexpr inline auto y = title::y;
        constexpr inline auto w = 60;
        constexpr inline auto h = title::h;

        constexpr inline auto margin = 3;

        namespace text
        {
            constexpr inline auto w = 40;
        }

    } // namespace ice

    namespace primary_text
    {
        constexpr inline auto x = style::window::default_left_margin;
        constexpr inline auto y = 294;
        constexpr inline auto w = style::window_width - 2 * x;
        constexpr inline auto h = 60;
    } // namespace primary_text

    namespace secondary_text
    {
        constexpr inline auto x = style::window::default_left_margin;
        constexpr inline auto y = primary_text::y + primary_text::h + 30;
        constexpr inline auto w = style::window_width - 2 * x;
        constexpr inline auto h = 90;
    } // namespace secondary_text

} // namespace style::window::pin_lock

namespace style::window::screen_pin_lock
{
    namespace pin_label
    {
        constexpr inline auto y = 248;
    } // namespace pin_label

    namespace primary_text
    {
        constexpr inline auto y = 160;
    } // namespace primary_text
} // namespace style::window::screen_pin_lock
