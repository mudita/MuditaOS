// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace style::header::status_bar
{
    namespace font
    {

        inline constexpr auto title = "gt_pressura_bold_32";
    }; // namespace font

    namespace margins
    {
        inline constexpr auto iconBottom = 4u;
        inline constexpr auto textBottom = 5u;
        inline constexpr auto between    = 10u;
    }; // namespace margins
    namespace time
    {
        inline constexpr auto font = "gt_pressura_regular_24";
        inline constexpr auto maxX = 100u;
    }; // namespace time
    namespace nat
    {
        inline constexpr auto maxX = 35u;
        inline constexpr auto font = "gt_pressura_regular_20";
    }; // namespace nat
    namespace phonemode
    {
        inline constexpr auto maxX = 157u;
        inline constexpr auto font = "gt_pressura_regular_20";
    }; // namespace phonemode
    namespace signal
    {
        inline constexpr auto font = "gt_pressura_regular_20";
        inline constexpr auto maxX = 80u;
    }; // namespace signal
    namespace battery
    {
        inline constexpr auto font = "gt_pressura_regular_20";
        inline constexpr auto maxX = 70U;
    }; // namespace battery
    namespace boxes
    {
        namespace left
        {
            inline constexpr auto minX = 165u;
            inline constexpr auto maxX = 205u;
        }; // namespace left
        namespace right
        {
            inline constexpr auto minX = 165u;
            inline constexpr auto maxX = 205u;
        }; // namespace right
        namespace center
        {
            inline constexpr auto maxX = time::maxX;
            inline constexpr auto minX = 30;
        }; // namespace center
    };     // namespace boxes

}; // namespace style::header::status_bar
