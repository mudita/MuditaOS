#pragma once
#include <Style.hpp>
#include <utf8/UTF8.hpp>

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
        } // namespace contactsList
    }     // namespace mainWindow

    namespace newContactWindow
    {
        namespace newContactsList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace newContactsList
    }     // namespace newContactWindow

    namespace contactDetailsWindow
    {
        namespace contactDetailsList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height + 74;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactDetailsList
        namespace contactDetailsListNoFlags
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactDetailsListNoFlags

    } // namespace contactDetailsWindow

    namespace searchResultsWindow
    {
        namespace searchResultList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace searchResultList
    }     // namespace searchResultsWindow

    namespace iceContactsWindow
    {
        namespace contactsListIce
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
            constexpr uint32_t h = style::window_height - y - style::footer::height;
        } // namespace contactsListIce
    }     // namespace iceContactsWindow

    namespace contactItem
    {
        constexpr uint32_t w                    = style::window::default_body_width;
        constexpr uint32_t h                    = style::window::label::big_h;
        constexpr uint32_t blocked_right_margin = 5;
        const UTF8 favourites_string            = "Favourites";
    } // namespace contactItem

    namespace informationWidget
    {
        constexpr uint32_t w             = style::window::default_body_width;
        constexpr uint32_t title_label_h = 20;
        constexpr uint32_t email_text_h  = 35;
    } // namespace informationWidget

    namespace inputBoxWithLabelAndIconIWidget
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = 50;

        constexpr uint32_t input_box_w           = 55;
        constexpr uint32_t input_box_h           = h;
        constexpr int32_t input_box_right_margin = 20;

        constexpr uint32_t description_label_w           = 280;
        constexpr uint32_t description_label_h           = 33;
        constexpr int32_t description_label_right_margin = 40;

        constexpr int32_t tick_image_left_margin  = -64;
        constexpr int32_t tick_image_right_margin = 32;

    } // namespace inputBoxWithLabelAndIconIWidget

    namespace inputLinesWithLabelWidget
    {
        constexpr uint32_t w                = style::window::default_body_width;
        constexpr uint32_t h                = 63;
        constexpr uint32_t title_label_h    = 20;
        constexpr uint32_t input_text_h     = 37;
        constexpr uint32_t span_size        = 6;
        constexpr int32_t underline_padding = 4;
    } // namespace inputLinesWithLabelWidget

    namespace outputLinesTextWithLabelWidget
    {
        constexpr uint32_t w             = style::window::default_body_width;
        constexpr uint32_t h             = 75;
        constexpr uint32_t title_label_h = 20;
        constexpr uint32_t input_text_h  = 33;
        constexpr uint32_t span_size     = style::margins::huge;
    } // namespace outputLinesTextWithLabelWidget

    namespace numbersWithIconsWidget
    {
        constexpr uint32_t h                        = 55;
        constexpr uint32_t sms_image_w              = 55;
        constexpr uint32_t sms_image_h              = h;
        constexpr uint32_t phone_image_w            = 55;
        constexpr uint32_t phone_image_h            = h;
        constexpr uint32_t phone_image_margin_left  = 30;
        constexpr uint32_t phone_image_margin_right = 15;
        constexpr uint32_t number_text_h            = h;
    } // namespace numbersWithIconsWidget

} // namespace phonebookStyle
