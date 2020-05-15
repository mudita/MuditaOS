#pragma once
#include <Style.hpp>

namespace phonebookStyle
{
    namespace mainWindow
    {
        constexpr uint32_t default_x = style::window::default_left_margin;
        constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        namespace leftArrowImage
        {
            constexpr uint32_t x = default_x;
            constexpr uint32_t y = 62;
            constexpr uint32_t w = 11;
            constexpr uint32_t h = 13;
        } // namespace leftArrowImage
        namespace rightArrowImage
        {
            constexpr uint32_t x = style::window_width - style::window::default_left_margin - 11;
            constexpr uint32_t y = 62;
            constexpr uint32_t w = 11;
            constexpr uint32_t h = 13;
        } // namespace rightArrowImage
        namespace newContactImage
        {
            constexpr uint32_t x = style::window::default_left_margin + 20;
            constexpr uint32_t y = 55;
            constexpr uint32_t w = 24;
            constexpr uint32_t h = 24;
        } // namespace newContactImage
        namespace searchImage
        {
            constexpr uint32_t x =
                style::window_width - style::window::default_left_margin - rightArrowImage::w - 8 - 26;
            constexpr uint32_t y = 55;
            constexpr uint32_t w = 26;
            constexpr uint32_t h = 26;
        } // namespace searchImage
        namespace contactsList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height + style::listview::top_margin_big;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;

            constexpr uint32_t maxElements   = 7;
            constexpr uint32_t pageSize      = 8;
            constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
            constexpr uint32_t penWidth      = 0;
        } // namespace contactsList
    }     // namespace mainWindow
    namespace searchResults
    {
        constexpr uint32_t default_x = style::window::default_left_margin;
        constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        namespace searchResultList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height + 6;
            constexpr uint32_t w =
                style::window_width - style::window::default_left_margin - style::window::default_right_margin;
            constexpr uint32_t h             = style::window_height - y - 50;
            constexpr uint32_t maxElements   = 7;
            constexpr uint32_t pageSize      = 7;
            constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
            constexpr uint32_t penWidth      = 0;
        } // namespace searchResultList
    }     // namespace searchResults
} // namespace phonebookStyle
