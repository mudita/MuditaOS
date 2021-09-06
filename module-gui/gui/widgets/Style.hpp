// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "screen.hpp"
#include <limits>
#include <gui/core/Color.hpp>
#include <gui/Common.hpp>
#include <Alignment.hpp>
#include <Margins.hpp>

namespace gui
{
    class Rect;
    class Label;
}; // namespace gui

/// one place to gather all common magical numbers from design
namespace style
{
    inline constexpr auto window_height = getScreenResolutionY();
    inline constexpr auto window_width  = getScreenResolutionX();

    namespace status_bar
    {
        inline constexpr auto default_horizontal_pos  = 20U;
        inline constexpr auto default_vertical_pos    = 0U;
        inline constexpr unsigned status_bar_margin_w = default_horizontal_pos * 2;
        inline constexpr auto height                  = 46U;
        inline constexpr auto width                   = window_width - status_bar_margin_w;
    } // namespace status_bar

    namespace header
    {
        inline constexpr auto default_horizontal_pos = 0U;
        inline constexpr auto default_vertical_pos   = status_bar::height;
        inline constexpr auto height                 = 59U;
        inline constexpr auto width                  = window_width;

        namespace font
        {
            inline constexpr auto title = "gt_pressura_bold_30";
        } // namespace font
    }     // namespace header

    namespace footer
    {
        inline constexpr auto height = 54U;
        namespace font
        {
            inline constexpr auto bold   = "gt_pressura_bold_24";
            inline constexpr auto medium = "gt_pressura_regular_24";
        }; // namespace font
    };     // namespace footer

    namespace window
    {
        inline constexpr auto default_vertical_pos = header::default_vertical_pos + header::height;
        inline constexpr auto default_left_margin  = 20U;
        inline constexpr auto default_right_margin = 20U;
        inline constexpr auto default_body_width =
            style::window_width - style::window::default_right_margin - style::window::default_left_margin;
        inline constexpr auto default_body_height =
            style::window_height - style::window::default_vertical_pos - style::footer::height;
        inline constexpr auto default_border_focus_w       = 2U;
        inline constexpr auto default_border_rect_no_focus = 1U;
        inline constexpr auto default_border_no_focus_w    = 0U;
        inline constexpr auto default_rect_yaps            = 10U;
        namespace font
        {
            inline constexpr auto huge             = "gt_pressura_regular_190";
            inline constexpr auto supersizeme      = "gt_pressura_regular_90";
            inline constexpr auto supersizemelight = "gt_pressura_light_90";
            inline constexpr auto largelight       = "gt_pressura_light_46";
            inline constexpr auto large            = "gt_pressura_regular_46";
            inline constexpr auto verybiglight     = "gt_pressura_light_38";
            inline constexpr auto bigbold          = "gt_pressura_bold_30";
            inline constexpr auto big              = "gt_pressura_regular_30";
            inline constexpr auto biglight         = "gt_pressura_light_30";
            inline constexpr auto mediumbold       = "gt_pressura_bold_27";
            inline constexpr auto medium           = "gt_pressura_regular_27";
            inline constexpr auto mediumlight      = "gt_pressura_light_27";
            inline constexpr auto smallbold        = "gt_pressura_bold_24";
            inline constexpr auto small            = "gt_pressura_regular_24";
            inline constexpr auto verysmallbold    = "gt_pressura_bold_20";
            inline constexpr auto verysmall        = "gt_pressura_regular_20";
        }; // namespace font

        inline constexpr auto list_offset_default = 12U;
        namespace label
        {
            inline constexpr auto small_h   = 33U;
            inline constexpr auto default_h = 50U;
            inline constexpr auto big_h     = 55U;
        }; // namespace label

        /// minimal label decoration - edges, focus & alignment
        void decorate(gui::Rect *el);
        void decorate(gui::Label *el);
        /// minimal label decoration for Option
        void decorateOption(gui::Label *el);

        namespace bottomBar
        {
            inline constexpr auto leftMargin  = 30U;
            inline constexpr auto rightMargin = 30U;

            inline constexpr auto h = 54U;
            inline constexpr auto w = window_width;
        } // namespace bottomBar

        namespace progressBar
        {
            inline constexpr auto x     = style::window::default_left_margin;
            inline constexpr auto y     = 400U;
            inline constexpr auto w     = style::window::default_body_width;
            inline constexpr auto h     = 50U;
            inline constexpr auto range = 10U;
        }; // namespace progressBar
    };     // namespace window

    namespace settings
    {
        namespace date
        {
            inline constexpr auto date_time_item_height       = 107U;
            inline constexpr auto date_time_item_width        = 120U;
            inline constexpr auto date_time_item_title_height = 30U;
            inline constexpr auto date_time_spacer_width      = 20U;
            inline constexpr auto date_time_x_offset          = 30U;

            inline constexpr auto date_time_box_h = 107U;
            inline constexpr auto date_box_y_pos  = 123U;
            inline constexpr auto time_box_y_pos  = 285U;

        } // namespace date
        namespace ussd
        {
            inline constexpr auto commonXPos = 40U;
            inline constexpr auto commonYPos = 110U;

            inline constexpr auto commonW      = 420U;
            inline constexpr auto commonLabelH = 33U;
            inline constexpr auto commonTextH  = 99U;
        } // namespace ussd
    }     // namespace settings
    namespace color
    {
        inline constexpr auto lightgrey = gui::Color(3, 0);
    }; //  namespace color
    namespace text
    {
        inline constexpr auto defaultTextAlignment =
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom);
        inline constexpr auto maxTextLines = 10;
    }; // namespace text

    namespace strings
    {
        namespace common
        {
            inline constexpr auto add            = "common_add";
            inline constexpr auto open           = "common_open";
            inline constexpr auto call           = "common_call";
            inline constexpr auto send           = "common_send";
            inline constexpr auto save           = "common_save";
            inline constexpr auto edit           = "common_edit";
            inline constexpr auto import         = "common_import";
            inline constexpr auto confirm        = "common_confirm";
            inline constexpr auto select         = "common_select";
            inline constexpr auto use            = "common_use";
            inline constexpr auto ok             = "common_ok";
            inline constexpr auto back           = "common_back";
            inline constexpr auto skip           = "common_skip";
            inline constexpr auto set            = "common_set";
            inline constexpr auto show           = "common_show";
            inline constexpr auto yes            = "common_yes";
            inline constexpr auto no             = "common_no";
            inline constexpr auto check          = "common_check";
            inline constexpr auto Switch         = "common_switch";
            inline constexpr auto options        = "common_options";
            inline constexpr auto information    = "common_information";
            inline constexpr auto search         = "common_search";
            inline constexpr auto search_results = "common_search_results";
            inline constexpr auto emoji          = "common_emoji";
            inline constexpr auto special_chars  = "common_special_characters";
            inline constexpr auto start          = "common_start";
            inline constexpr auto stop           = "common_stop";
            inline constexpr auto resume         = "common_resume";
            inline constexpr auto pause          = "common_pause";
            inline constexpr auto accept         = "common_accept";
            inline constexpr auto retry          = "common_retry";
            inline constexpr auto replace        = "common_replace";
            inline constexpr auto abort          = "common_abort";
            inline constexpr auto adjust         = "common_adjust";
            inline constexpr auto play_pause     = "common_play_pause";
            // days
            inline constexpr auto Monday    = "common_monday";
            inline constexpr auto Tuesday   = "common_tuesday";
            inline constexpr auto Wednesday = "common_wednesday";
            inline constexpr auto Thursday  = "common_thursday";
            inline constexpr auto Friday    = "common_friday";
            inline constexpr auto Saturday  = "common_saturday";
            inline constexpr auto Sunday    = "common_sunday";
            // months
            inline constexpr auto January   = "common_january";
            inline constexpr auto February  = "common_february";
            inline constexpr auto March     = "common_march";
            inline constexpr auto April     = "common_april";
            inline constexpr auto May       = "common_may";
            inline constexpr auto June      = "common_june";
            inline constexpr auto July      = "common_july";
            inline constexpr auto August    = "common_august";
            inline constexpr auto September = "common_september";
            inline constexpr auto October   = "common_october";
            inline constexpr auto November  = "common_november";
            inline constexpr auto December  = "common_december";
            inline constexpr auto Yesterday = "common_yesterday";
            inline constexpr auto Today     = "common_today";
        } // namespace common
    }     // namespace strings

    namespace listview
    {
        namespace scroll
        {
            inline constexpr auto x           = 0U;
            inline constexpr auto y           = 0U;
            inline constexpr auto w           = 5U;
            inline constexpr auto h           = 0U;
            inline constexpr auto radius      = 2U;
            inline constexpr auto color       = gui::Color{0, 0};
            inline constexpr auto margin      = 5U;
            inline constexpr auto min_space   = 10U;
            inline constexpr auto item_margin = 10U;
        } // namespace scroll

        inline constexpr auto item_width_with_scroll =
            style::window::default_body_width - style::listview::scroll::item_margin;
        inline constexpr auto item_width_with_without_scroll =
            style::window::default_body_width - 2 * style::listview::scroll::item_margin;
        inline constexpr auto body_width_with_scroll =
            style::window::default_body_width + style::listview::scroll::margin;

    } // namespace listview

    namespace sidelistview
    {
        inline auto arrow_left_image  = "arrow_left_24px_W_G";
        inline auto arrow_right_image = "arrow_right_24px_W_G";

        namespace list_item
        {
            inline constexpr auto w = window_width;
            inline constexpr auto h = window_height;
        } // namespace list_item

        namespace progress_bar
        {
            inline constexpr auto margin_left   = 0U;
            inline constexpr auto margin_top    = 10U;
            inline constexpr auto margin_right  = 0U;
            inline constexpr auto margin_bottom = 10U;
            inline constexpr auto h             = 50U;
        } // namespace progress_bar

    } // namespace sidelistview

    namespace margins
    {
        inline constexpr auto very_small = 3U;
        inline constexpr auto small      = 6U;
        inline constexpr auto big        = 8U;
        inline constexpr auto very_big   = 12U;
        inline constexpr auto large      = 20U;
        inline constexpr auto huge       = 24U;
    } // namespace margins

    namespace padding
    {
        inline constexpr auto default_left_text_padding = 10U;
    } // namespace padding

    namespace widgets
    {
        inline constexpr auto h           = 55U;
        inline constexpr auto iconsSize   = h;
        inline constexpr auto leftMargin  = 10U;
        inline constexpr auto rightMargin = 10U;
    } // namespace widgets

    namespace notifications
    {
        inline constexpr auto spanSize     = 8;
        inline constexpr auto iconWidth    = 35;
        inline constexpr auto textMinWidth = 250;
        inline constexpr auto textMaxWidth = 350;
        inline constexpr auto itemHeight   = 55;

        namespace model
        {
            inline constexpr auto maxNotificationsPerPage = 4;
            inline constexpr auto x                       = 20;
            inline constexpr auto y                       = 284;
            inline constexpr auto w                       = style::window::default_body_width;
            inline constexpr auto h                       = itemHeight * maxNotificationsPerPage;
        } // namespace model
    }     // namespace notifications

}; // namespace style
