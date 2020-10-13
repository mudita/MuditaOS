#pragma once

#include <Alignment.hpp>
#include <Margins.hpp>
#include <gui/core/Color.hpp>
#include <inttypes.h>
#include <string>
#include <gui/Common.hpp>

namespace gui
{
    class Rect;
    class Label;
}; // namespace gui

/// one place to gather all common magical numbers from design
namespace style
{
    const inline uint32_t window_height = 600;
    const inline uint32_t window_width  = 480;
    namespace header
    {
        const inline uint32_t height = 105;
        namespace font
        {
            const inline std::string time  = "gt_pressura_regular_24";
            const inline std::string title = "gt_pressura_bold_32";
        }; // namespace font
    };     // namespace header

    namespace footer
    {
        const inline uint32_t height = 54;
        namespace font
        {
            const inline std::string bold   = "gt_pressura_bold_24";
            const inline std::string medium = "gt_pressura_regular_24";
        }; // namespace font
    };     // namespace footer

    namespace window
    {
        const inline uint32_t default_left_margin  = 20;
        const inline uint32_t default_right_margin = 20;
        const inline uint32_t default_body_width =
            style::window_width - style::window::default_right_margin - style::window::default_left_margin;
        const inline uint32_t default_body_height =
            style::window_height - style::header::height - style::footer::height;
        const inline uint32_t default_border_focus_w       = 2;
        const inline uint32_t default_border_rect_no_focus = 1;
        const inline uint32_t default_border_no_focus_w    = 0;
        const inline uint32_t default_rect_yaps            = 10;
        namespace font
        {
            const inline std::string supersizemelight = "gt_pressura_light_90";
            const inline std::string largelight       = "gt_pressura_light_46";
            const inline std::string verybigbold      = "gt_pressura_bold_32";
            const inline std::string bigbold          = "gt_pressura_bold_30";
            const inline std::string big              = "gt_pressura_regular_30";
            const inline std::string biglight         = "gt_pressura_light_30";
            const inline std::string mediumbold       = "gt_pressura_bold_27";
            const inline std::string medium           = "gt_pressura_regular_27";
            const inline std::string mediumlight      = "gt_pressura_light_27";
            const inline std::string smallbold        = "gt_pressura_bold_24";
            const inline std::string small            = "gt_pressura_regular_24";
            const inline std::string verysmallbold    = "gt_pressura_bold_20";
            const inline std::string verysmall        = "gt_pressura_regular_20";
        }; // namespace font

        const inline uint32_t list_offset_default = 12;
        namespace label
        {
            const inline uint32_t small_h   = 33;
            const inline uint32_t default_h = 50;
            const inline uint32_t big_h     = 55;
        }; // namespace label

        /// minimal label decoration - edges, focus & alignment
        void decorate(gui::Rect *el);
        void decorate(gui::Label *el);
        /// minimal label decoration for Option
        void decorateOption(gui::Label *el);

    }; // namespace window

    namespace settings
    {
        namespace date
        {
            const inline uint32_t date_time_item_height       = 107;
            const inline uint32_t date_time_item_width        = 120;
            const inline uint32_t date_time_item_title_height = 30;
            const inline uint32_t date_time_spacer_width      = 20;
            const inline uint32_t date_time_x_offset          = 30;

            const inline uint32_t date_time_box_h = 107;
            const inline uint32_t date_box_y_pos  = 123;
            const inline uint32_t time_box_y_pos  = 285;

        } // namespace date
        namespace ussd
        {
            constexpr uint32_t commonXPos = 40;
            constexpr uint32_t commonYPos = 110;

            constexpr uint32_t commonW      = 420;
            constexpr uint32_t commonLabelH = 33;
            constexpr uint32_t commonTextH  = 99;
        } // namespace ussd
    }     // namespace settings
    namespace color
    {
        const inline gui::Color lightgrey = gui::Color(3, 0);
    }; //  namespace color
    namespace text
    {
        const inline gui::Alignment defaultTextAlignment =
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom);
        const inline unsigned int maxTextLines = 10;
    }; // namespace text

    namespace strings
    {
        namespace common
        {
            const inline std::string open           = "common_open";
            const inline std::string call           = "common_call";
            const inline std::string send           = "common_send";
            const inline std::string save           = "common_save";
            const inline std::string confirm        = "common_confirm";
            const inline std::string select         = "common_select";
            const inline std::string use            = "common_use";
            const inline std::string ok             = "common_ok";
            const inline std::string back           = "common_back";
            const inline std::string set            = "common_set";
            const inline std::string yes            = "common_yes";
            const inline std::string no             = "common_no";
            const inline std::string Switch         = "common_switch";
            const inline std::string options        = "common_options";
            const inline std::string information    = "common_information";
            const inline std::string search         = "common_search";
            const inline std::string search_results = "common_search_results";
            const inline std::string emoji          = "common_emoji";
            const inline std::string special_chars  = "common_special_characters";
            // days
            const inline std::string Monday    = "common_monday";
            const inline std::string Tuesday   = "common_tuesday";
            const inline std::string Wednesday = "common_wednesday";
            const inline std::string Thursday  = "common_thursday";
            const inline std::string Friday    = "common_friday";
            const inline std::string Saturday  = "common_saturday";
            const inline std::string Sunday    = "common_sunday";
            // months
            const inline std::string January   = "common_january";
            const inline std::string February  = "common_february";
            const inline std::string March     = "common_march";
            const inline std::string April     = "common_april";
            const inline std::string May       = "common_may";
            const inline std::string June      = "common_june";
            const inline std::string July      = "common_july";
            const inline std::string August    = "common_august";
            const inline std::string September = "common_september";
            const inline std::string October   = "common_october";
            const inline std::string November  = "common_november";
            const inline std::string December  = "common_december";
            const inline std::string Yesterday = "common_yesterday";
            const inline std::string Today     = "common_today";
        } // namespace common
    }     // namespace strings

    namespace listview
    {
        /// Possible List boundaries handling types
        enum class Boundaries
        {
            Fixed,     ///< Fixed - list will stop scrolling on first or last elements on appropriate top or bottom
                       ///< directions.
            Continuous ///< Continuous - list will continue to beginning or end on first or last elements on
                       ///< appropriate top or bottom directions.
        };

        /// Possible List scrolling directions
        enum class Direction
        {
            Top,
            Bottom
        };

        /// Possible List rebuild types
        enum class RebuildType
        {
            Full,    ///< Full rebuild - resets lists to all initial conditions and request data from beginning.
            InPlace, ///< InPlace rebuild - stores currently focused part of list and rebuild from that part.
            OnOffset ///< OnOffset rebuild - resets lists to all initial conditions and request data from provided
                     ///< offset.
        };

        enum class Orientation
        {
            TopBottom,
            BottomTop
        };

        namespace scroll
        {
            const inline uint32_t x           = 0;
            const inline uint32_t y           = 0;
            const inline uint32_t w           = 5;
            const inline uint32_t h           = 0;
            const inline uint32_t radius      = 2;
            const inline gui::Color color     = gui::Color{0, 0};
            const inline uint32_t margin      = 5;
            const inline uint32_t min_space   = 10;
            const inline uint32_t item_margin = 10;
        } // namespace scroll

        const inline uint32_t item_width_with_scroll =
            style::window::default_body_width - style::listview::scroll::item_margin;
        const inline uint32_t body_width_with_scroll =
            style::window::default_body_width + style::listview::scroll::margin;
        const inline uint32_t right_margin        = 15;
        const inline uint32_t top_margin_small    = 5;
        const inline uint32_t top_margin_big      = 8;
        const inline uint32_t top_margin_very_big = 12;
        const inline uint32_t item_span_small     = 8;
        const inline uint32_t item_span_big       = 12;

    } // namespace listview

    namespace margins
    {
        const inline uint32_t small    = 6;
        const inline uint32_t big      = 8;
        const inline uint32_t very_big = 12;
        const inline uint32_t huge     = 24;
    } // namespace margins

    namespace padding
    {
        const inline uint32_t default_left_text_padding = 10;
    } // namespace padding

}; // namespace style
