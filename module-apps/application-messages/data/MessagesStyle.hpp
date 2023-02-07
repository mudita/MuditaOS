// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Style.hpp>

#include <cstdint>

namespace style
{
    namespace messages
    {
        namespace threads
        {
            inline constexpr uint32_t listPositionX = style::window::default_left_margin;
            // Magic 1 -> discussed with Design for proper alignment.
            inline constexpr uint32_t ListPositionY = style::window::default_vertical_pos - 1;
            // Bottom margin need to be added to fit all elements.
            inline constexpr uint32_t listHeight =
                style::window_height - ListPositionY - style::nav_bar::height + style::margins::small;
            inline constexpr uint32_t listWidth = style::listview::body_width_with_scroll;
        } // namespace threads

        namespace threadItem
        {
            inline constexpr uint32_t sms_thread_item_h = 100;

            inline constexpr uint32_t topMargin    = 16;
            inline constexpr uint32_t bottomMargin = 13;

            inline constexpr uint32_t leftMargin  = 10;
            inline constexpr uint32_t rightMargin = 10;

            inline constexpr uint32_t timestampWidth             = 132;
            inline constexpr uint32_t numberImportanceWidth      = 80;
            inline constexpr uint32_t numberImportanceLeftMargin = 10;
            inline constexpr uint32_t snippetLeftMargin          = 5;
            inline constexpr uint32_t cotactWidthOffset          = timestampWidth + leftMargin + rightMargin;
            inline constexpr uint32_t notSentIconWidth           = 20;

            inline constexpr uint32_t previewWidthOffset = leftMargin + rightMargin + 10;
        } // namespace threadItem

        namespace newMessage
        {
            namespace recipientLabel
            {
                inline constexpr uint32_t h = 42;
            }
            namespace recipientImg
            {
                inline constexpr uint32_t w = 32, h = 32;
            }
            namespace text
            {
                inline constexpr uint32_t h    = 43;
                inline constexpr uint32_t maxH = 320;
            } // namespace text
            namespace messageLabel
            {
                inline constexpr uint32_t h = 44;
            }
        } // namespace newMessage

        namespace smsInput
        {
            inline constexpr gui::Length min_h                   = 40;
            inline constexpr gui::Length default_input_w         = 385;
            inline constexpr gui::Length default_input_h         = 30;
            inline constexpr gui::Length bottom_padding          = 5;
            inline constexpr gui::Length max_input_h             = default_input_h * 4 + bottom_padding;
            inline constexpr gui::Length reply_bottom_margin     = 8;
            inline constexpr gui::Length new_sms_vertical_spacer = 25;
            inline constexpr gui::Length new_sms_left_margin     = 10;
        } // namespace smsInput

        namespace smsOutput
        {
            inline constexpr gui::Length sms_label_high              = 44;
            inline constexpr gui::Length sms_radius                  = 8;
            inline constexpr gui::Length default_h                   = 30;
            inline constexpr gui::Length sms_max_width               = 320;
            inline constexpr gui::Length sms_max_height              = 270;
            inline constexpr gui::Length sms_h_padding               = 15;
            inline constexpr gui::Length sms_h_big_padding           = 25;
            inline constexpr gui::Length sms_v_padding               = 10;
            inline constexpr gui::Length sms_vertical_spacer         = 10;
            inline constexpr gui::Length sms_error_icon_left_margin  = 5;
            inline constexpr gui::Length sms_error_icon_right_margin = 2;
            inline constexpr gui::Padding sms_left_bubble_padding    = gui::Padding(smsOutput::sms_h_big_padding,
                                                                                 smsOutput::sms_v_padding,
                                                                                 smsOutput::sms_h_padding,
                                                                                 smsOutput::sms_v_padding);
            inline constexpr gui::Padding sms_right_bubble_padding   = gui::Padding(smsOutput::sms_h_padding,
                                                                                  smsOutput::sms_v_padding,
                                                                                  smsOutput::sms_h_big_padding,
                                                                                  smsOutput::sms_v_padding);
        } // namespace smsOutput

        namespace smsList
        {
            inline constexpr uint32_t x = style::window::default_left_margin;
            inline constexpr uint32_t y = style::window::default_vertical_pos;
            inline constexpr uint32_t h = style::window::default_body_height;
            inline constexpr uint32_t w = style::listview::body_width_with_scroll;
        } // namespace smsList

        namespace templates
        {
            namespace list
            {
                inline constexpr uint32_t x = style::window::default_left_margin;
                inline constexpr uint32_t y = style::window::default_vertical_pos;
                inline constexpr uint32_t h = style::window_height - y - style::nav_bar::height;
                inline constexpr uint32_t w = style::listview::body_width_with_scroll;

            } // namespace list
        }     // namespace templates
    }         // namespace messages
} // namespace style
