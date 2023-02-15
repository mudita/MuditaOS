// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::status_bar
{
#if DEVELOPER_SETTINGS_OPTIONS == 1
    extern gui::ImageTypeSpecifier imageTypeSpecifier;
#else
    inline constexpr auto imageTypeSpecifier = gui::ImageTypeSpecifier::W_G;
#endif

    namespace margins
    {
        inline constexpr auto iconBottom = 4u;
        inline constexpr auto textBottom = 4u;
        inline constexpr auto between    = 10u;
        inline constexpr auto tethering  = 2u;
    }; // namespace margins
    namespace time
    {
        inline constexpr auto font          = style::window::font::small;
        inline constexpr auto maxX          = 100u;
        inline constexpr auto bottomPadding = 5u;
    }; // namespace time
    namespace nat
    {
        inline constexpr auto maxX = 35u;
        inline constexpr auto font = style::window::font::verysmall;
    }; // namespace nat
    namespace phonemode
    {
        inline constexpr auto maxX = 157u;
        inline constexpr auto font = style::window::font::verysmall;
    }; // namespace phonemode
    namespace signal
    {
        inline constexpr auto font = style::window::font::verysmall;
        inline constexpr auto maxX = 80u;
    }; // namespace signal
    namespace battery
    {
        inline constexpr auto font = style::window::font::verysmall;
        inline constexpr auto maxX = 70U;
    }; // namespace battery
    namespace boxes
    {
        namespace left
        {
            inline constexpr auto minX = 170u;
            inline constexpr auto maxX = 205u;
        }; // namespace left
        namespace right
        {
            inline constexpr auto minX = 170u;
            inline constexpr auto maxX = 205u;
        }; // namespace right
        namespace center
        {
            inline constexpr auto maxX = time::maxX;
            inline constexpr auto minX = 30;
        }; // namespace center
    };     // namespace boxes

}; // namespace style::status_bar
