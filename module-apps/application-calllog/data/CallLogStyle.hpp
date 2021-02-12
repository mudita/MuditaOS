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
        constexpr uint32_t default_x = style::window::default_left_margin;
        constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        namespace information
        {
            namespace label
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = 111;
                constexpr uint32_t w = default_w;
            } // namespace label
            namespace number
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = 174;
                constexpr uint32_t w = default_w;
            } // namespace number
            namespace imgs
            {
                constexpr uint32_t y = 162;
                constexpr uint32_t w = 55;
                constexpr uint32_t h = 55;
                namespace call
                {
                    constexpr uint32_t x = 317;
                    namespace icon
                    {
                        constexpr uint32_t x = 11;
                        constexpr uint32_t y = 12;
                    } // namespace icon
                }     // namespace call
                namespace sms
                {
                    constexpr uint32_t x = 389;
                    namespace icon
                    {
                        constexpr uint32_t x = 11;
                        constexpr uint32_t y = 12;
                    } // namespace icon
                }     // namespace sms
            }         // namespace imgs
        }             // namespace information
        namespace type
        {
            namespace label
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = 222;
                constexpr uint32_t w = style::window_width / 2 - x;
            } // namespace label
            namespace img
            {
                const uint32_t x = default_x;
                const uint32_t y = 285;
            } // namespace img
            namespace data
            {
                constexpr uint32_t x = 70;
                constexpr uint32_t y = 285;
                constexpr uint32_t w = style::window_width / 2 - x;
            } // namespace data
        }     // namespace type
        namespace duration
        {
            namespace label
            {
                constexpr uint32_t x = 281;
                constexpr uint32_t y = 222;
                constexpr uint32_t w = style::window_width - x - style::window::default_right_margin;
            } // namespace label
            namespace data
            {
                constexpr uint32_t x = 281;
                constexpr uint32_t y = 285;
                constexpr uint32_t w = style::window_width / 2 - style::window::default_right_margin;
            } // namespace data
        }     // namespace duration
        namespace date
        {
            namespace label
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = 333;
                constexpr uint32_t w = default_w;
            } // namespace label
            namespace dataDay
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = 396;
                constexpr uint32_t w = default_w;
            } // namespace dataDay
            namespace dataDate
            {
                constexpr uint32_t x = default_x;
                constexpr uint32_t y = dataDay::y + style::window::label::small_h;
                constexpr uint32_t w = default_w;
            } // namespace dataDate
        }     // namespace date
    }         // namespace detailsWindow

    // MAIN WINDOW
    namespace mainWindow
    {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = style::header::height;
        constexpr uint32_t w = style::listview::body_width_with_scroll;
        constexpr uint32_t h = style::window_height - y - style::footer::height;
    } // namespace mainWindow

} // namespace callLogStyle
