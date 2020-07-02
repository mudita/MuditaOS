#pragma once
#include <Style.hpp>
#include "utf8/UTF8.hpp"

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
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;

            constexpr uint32_t pageSize      = 8;
            constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
            constexpr uint32_t penWidth      = 0;
        } // namespace contactsList
    }     // namespace mainWindow

    namespace contactItem
    {
        constexpr uint32_t w           = style::window::default_body_width;
        constexpr uint32_t h           = style::window::label::big_h;
        constexpr uint32_t left_margin = 10;
        const UTF8 favourites_string   = "Favourites";
    } // namespace contactItem

    namespace inputLineWithLabelItem
    {
        constexpr uint32_t w                           = style::window::default_body_width;
        constexpr uint32_t h                           = 65;
        constexpr uint32_t title_label_h               = 20;
        constexpr uint32_t input_text_h                = 30;
        constexpr uint32_t title_label_input_text_span = h - input_text_h;
    } // namespace inputLineWithLabelItem

    namespace inputBoxWithLabelAndIconItem
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = 65;

        constexpr uint32_t input_box_w = 55;
        constexpr uint32_t input_box_h = 40;

        constexpr uint32_t description_label_x = input_box_w + 20;
        constexpr uint32_t description_label_w = 280;
        constexpr uint32_t description_label_h = 40;

        constexpr uint32_t icon_image_x = description_label_x + description_label_w + 35;
        constexpr uint32_t icon_image_w = 30;
        constexpr uint32_t icon_image_h = 30;

        constexpr uint32_t tick_image_x = 12;
        constexpr uint32_t tick_image_w = 30;
        constexpr uint32_t tick_image_h = 30;

    } // namespace inputBoxWithLabelAndIconItem

    namespace searchResults
    {
        namespace searchResultList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;

            constexpr uint32_t pageSize      = 8;
            constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
            constexpr uint32_t penWidth      = 0;
        } // namespace searchResultList
    }     // namespace searchResults
} // namespace phonebookStyle
