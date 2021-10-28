// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>

// CALL LOG STYLE
namespace callLogStyle
{
    namespace strings
    {
        inline constexpr auto privateNumber = "app_call_private_number";
    }
    // DETAILS WINDOW
    namespace detailsWindow
    {
        constexpr inline auto x = style::nav_bar::left_margin;
        constexpr inline auto y = style::window::default_vertical_pos;
        constexpr inline auto w = style::window_width - 2 * x;
        constexpr inline auto h = style::window::default_body_height;

        namespace widget
        {
            constexpr inline auto h      = style::widgets::h;
            constexpr inline auto smallH = style::window::label::small_h;
        } // namespace widget

        namespace callData
        {
            constexpr inline auto columns = 2;
            constexpr inline auto rows    = 2;
            constexpr inline auto h       = rows * widget::h;
        } // namespace callData

        namespace date
        {
            constexpr inline auto rows = 2;
            constexpr inline auto h    = rows * widget::smallH;
        } // namespace date

        namespace noCalls
        {
            constexpr inline auto infoHeight                = 33;
            constexpr inline auto infoTopMargin             = 195;
            constexpr inline auto divLineTopMargin          = 90;
            constexpr inline auto bottomBoxWidth            = 332;
            constexpr inline auto bottomBoxHeight           = 132;
            constexpr inline auto bottomBoxMargin           = 72;
            constexpr inline auto infoIconMargin            = 2;
            constexpr inline auto descriptionSize           = 32;
            constexpr inline auto descriptionLeftMargin     = 55;
            constexpr inline auto descriptionInternalMargin = 5;
            constexpr inline auto descriptionTextWidth      = 105;
        } // namespace noCalls
    }     // namespace detailsWindow

    // MAIN WINDOW
    namespace mainWindow
    {
        constexpr inline uint32_t x = style::window::default_left_margin;
        constexpr inline uint32_t y = style::window::default_vertical_pos;
        constexpr inline uint32_t w = style::listview::body_width_with_scroll;
        constexpr inline uint32_t h = style::window_height - y - style::nav_bar::height;
    } // namespace mainWindow

} // namespace callLogStyle
