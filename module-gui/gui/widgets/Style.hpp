#pragma once

#include <Alignment.hpp>
#include <gui/core/Color.hpp>
#include <inttypes.h>
#include <string>

namespace gui
{
    class Label;
};

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
    namespace window
    {
        const inline uint32_t default_left_margin       = 30;
        const inline uint32_t default_right_margin      = 30;
        const inline uint32_t default_border_focucs_w   = 2;
        const inline uint32_t default_border_no_focus_w = 0;
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
        void decorate(gui::Label *el);
        /// minimal label decoration for Option
        void decorateOption(gui::Label *el);

        namespace messages
        {
            inline const uint32_t sms_radius          = 7;
            inline const uint32_t sms_border_no_focus = 1;
            /// TODO 100 is static size, sms elements should have size depending on text amount
            inline const uint32_t sms_height                = 100;
            const inline unsigned short yaps_size_default   = 10;
            const inline unsigned short sms_max_width       = 320;
            const inline unsigned short sms_h_padding       = 15;
            const inline unsigned short sms_v_padding       = 10;
            const inline unsigned short sms_vertical_spacer = 10;
        } // namespace messages

    }; // namespace window
    namespace footer
    {
        const inline uint32_t height = 54;
        namespace font
        {
            const inline std::string bold   = "gt_pressura_bold_24";
            const inline std::string medium = "gt_pressura_regular_24";
        }; // namespace font
    };     // namespace footer
    namespace settings
    {
        namespace date
        {
            const inline uint32_t date_time_item_height       = 107;
            const inline uint32_t date_time_item_width        = 120;
            const inline uint32_t date_time_item_title_height = 30;
            const inline uint32_t date_time_spacer_width      = 20;
            const inline uint32_t date_time_x_offset          = 30;
        } // namespace date
    }     // namespace settings
    namespace color
    {
        const inline gui::Color lightgrey = gui::Color(3, 0);
    }; //  namespace color
    namespace text
    {
        const inline gui::Alignment defaultTextAlignment =
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM);
    };

    namespace strings
    {
        namespace common
        {
            const inline std::string open        = "common_open";
            const inline std::string call        = "common_call";
            const inline std::string send        = "common_send";
            const inline std::string confirm     = "common_confirm";
            const inline std::string select      = "common_select";
            const inline std::string ok          = "common_ok";
            const inline std::string back        = "common_back";
            const inline std::string set         = "common_set";
            const inline std::string yes         = "common_yes";
            const inline std::string no          = "common_no";
            const inline std::string Switch      = "common_switch";
            const inline std::string options     = "common_options";
            const inline std::string information = "common_information";
            const inline std::string search      = "common_search";
            // days
            const inline std::string Monday    = "common_monday";
            const inline std::string Tuesday   = "common_tuesday";
            const inline std::string Wednesday = "common_wendesday";
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
        } // namespace common
    }     // namespace strings
}; // namespace style
