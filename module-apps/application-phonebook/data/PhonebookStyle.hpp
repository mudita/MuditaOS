// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>
#include <utf8/UTF8.hpp>

namespace phonebookStyle
{
    namespace mainWindow
    {
        namespace leftArrowImage
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = 62;
            inline constexpr uint32_t w = 11;
            inline constexpr uint32_t h = 13;
        } // namespace leftArrowImage
        namespace rightArrowImage
        {
            inline constexpr uint32_t w = 11;
            inline constexpr uint32_t h = 13;
            inline constexpr uint32_t x = style::window_width - style::window::default_right_margin - w;
            inline constexpr uint32_t y = 62;
        } // namespace rightArrowImage
        namespace newContactImage
        {
            inline constexpr uint32_t distance_to_arrow = 6;
            inline constexpr uint32_t x = style::window::default_left_margin + leftArrowImage::w + distance_to_arrow;
            inline constexpr uint32_t y = 52;
            inline constexpr uint32_t w = 24;
            inline constexpr uint32_t h = 24;
        } // namespace newContactImage
        namespace searchImage
        {
            inline constexpr uint32_t w = 26;
            inline constexpr uint32_t h = 26;
            inline constexpr uint32_t distance_to_arrow = 11;
            inline constexpr uint32_t x =
                style::window_width - style::window::default_right_margin - rightArrowImage::w - w - distance_to_arrow;
            inline constexpr uint32_t y = 55;
        } // namespace searchImage
        namespace contactsList
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactsList
    }     // namespace mainWindow

    namespace newContactWindow
    {
        namespace newContactsList
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace newContactsList
    }     // namespace newContactWindow

    namespace contactDetailsWindow
    {
        namespace contactDetailsList
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height + 74;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactDetailsList
        namespace contactDetailsListNoFlags
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactDetailsListNoFlags

    } // namespace contactDetailsWindow

    namespace searchResultsWindow
    {
        namespace searchResultList
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace searchResultList
    }     // namespace searchResultsWindow

    namespace iceContactsWindow
    {
        namespace contactsListIce
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::header::height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
            inline constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactsListIce
    }     // namespace iceContactsWindow

    namespace contactItem
    {
        inline constexpr uint32_t w                    = style::window::default_body_width;
        inline constexpr uint32_t h                    = style::window::label::big_h;
        inline constexpr uint32_t blocked_right_margin = 5;
        inline constexpr auto favourites_string        = "Favourites";
    } // namespace contactItem

    namespace informationWidget
    {
        inline constexpr uint32_t w             = style::window::default_body_width;
        inline constexpr uint32_t title_label_h = 22;
        inline constexpr uint32_t email_text_h  = 35;
    } // namespace informationWidget

    namespace inputBoxWithLabelAndIconIWidget
    {
        inline constexpr uint32_t w = style::window::default_body_width;
        inline constexpr uint32_t h = 50;

        inline constexpr uint32_t input_box_w           = 55;
        inline constexpr uint32_t input_box_h           = h;
        inline constexpr int32_t input_box_right_margin = 20;

        inline constexpr uint32_t description_label_w           = 280;
        inline constexpr uint32_t description_label_h           = 33;
        inline constexpr int32_t description_label_right_margin = 40;

        inline constexpr int32_t tick_image_left_margin  = -64;
        inline constexpr int32_t tick_image_right_margin = 32;

    } // namespace inputBoxWithLabelAndIconIWidget

    namespace inputLinesWithLabelWidget
    {
        inline constexpr uint32_t w                = style::window::default_body_width;
        inline constexpr uint32_t h                = 63;
        inline constexpr uint32_t title_label_h    = 20;
        inline constexpr uint32_t input_text_h     = 37;
        inline constexpr uint32_t span_size        = 6;
        inline constexpr int32_t underline_padding = 4;
    } // namespace inputLinesWithLabelWidget

    namespace outputLinesTextWithLabelWidget
    {
        inline constexpr uint32_t w             = style::window::default_body_width;
        inline constexpr uint32_t h             = 75;
        inline constexpr uint32_t title_label_h = 22;
        inline constexpr uint32_t input_text_h  = 33;
        inline constexpr uint32_t span_size     = style::margins::huge;
    } // namespace outputLinesTextWithLabelWidget

    namespace numbersWithIconsWidget
    {
        inline constexpr uint32_t h                        = 55;
        inline constexpr uint32_t sms_image_w              = 55;
        inline constexpr uint32_t sms_image_h              = h;
        inline constexpr uint32_t phone_image_w            = 55;
        inline constexpr uint32_t phone_image_h            = h;
        inline constexpr uint32_t phone_image_margin_left  = 30;
        inline constexpr uint32_t phone_image_margin_right = 15;
        inline constexpr uint32_t number_text_h            = h;
    } // namespace numbersWithIconsWidget

} // namespace phonebookStyle
