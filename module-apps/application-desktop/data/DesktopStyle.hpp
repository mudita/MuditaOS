// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::desktop
{
    namespace timeLabel
    {
        inline constexpr auto X      = 0;
        inline constexpr auto Y      = 106;
        inline constexpr auto Width  = style::window_width;
        inline constexpr auto Height = 96;

    } // namespace timeLabel

    namespace dayLabel
    {
        inline constexpr auto X      = 0;
        inline constexpr auto Y      = 204;
        inline constexpr auto Width  = style::window_width;
        inline constexpr auto Height = 51;

    } // namespace dayLabel

    namespace image
    {
        inline constexpr uint32_t x = 176;
        inline constexpr uint32_t y = 132;

    } // namespace image

    namespace textupdate
    {
        inline constexpr uint32_t x = 120;
        inline constexpr uint32_t y = 250;
        inline constexpr uint32_t w = 250;
        inline constexpr uint32_t h = 100;

    } // namespace textupdate

    namespace percentlabel
    {
        inline constexpr uint32_t x = 0;
        inline constexpr uint32_t y = 450;
        inline constexpr uint32_t w = style::window_width;
        inline constexpr uint32_t h = 100;

    } // namespace percentlabel

} // namespace style::desktop
