#pragma once

namespace style
{
    namespace messages
    {
        namespace threads
        {
            constexpr uint32_t listPositionX = style::window::default_left_margin;
            // Magic 1 -> discussed with Design for proper alignment.
            constexpr uint32_t ListPositionY = style::header::height - 1;
            // Bottom margin need to be added to fit all elements.
            constexpr uint32_t listHeight =
                style::window_height - ListPositionY - style::footer::height + style::margins::small;
            constexpr uint32_t listWidth = style::listview::body_width_with_scroll;
        } // namespace threads

        namespace threadItem
        {
            constexpr uint32_t sms_thread_item_h = 100;

            constexpr uint32_t topMargin    = 16;
            constexpr uint32_t bottomMargin = 13;

            constexpr uint32_t leftMargin  = 10;
            constexpr uint32_t rightMargin = 10;

            constexpr uint32_t timestampWidth             = 100;
            constexpr uint32_t numberImportanceWidth      = 80;
            constexpr uint32_t numberImportanceLeftMargin = 10;
            constexpr uint32_t cotactWidthOffset          = timestampWidth + leftMargin + rightMargin;
            constexpr uint32_t notSentIconWidth           = 20;

            constexpr uint32_t previewWidthOffset = leftMargin + rightMargin + 10;
        } // namespace threadItem

        namespace newMessage
        {
            namespace recipientLabel
            {
                constexpr uint32_t h = 42;
            }
            namespace recipientImg
            {
                constexpr uint32_t w = 32, h = 32;
            }
            namespace text
            {
                constexpr uint32_t h = 43;
            }
            namespace messageLabel
            {
                constexpr uint32_t h = 44;
            }
        } // namespace newMessage

        namespace smsInput
        {
            constexpr gui::Length min_h                   = 40;
            constexpr gui::Length default_input_w         = 395;
            constexpr gui::Length default_input_h         = 30;
            constexpr gui::Length bottom_padding          = 5;
            constexpr gui::Length max_input_h             = default_input_h * 4 + bottom_padding;
            constexpr gui::Length reply_bottom_margin     = 5;
            constexpr gui::Length new_sms_vertical_spacer = 25;
        } // namespace smsInput

        namespace smsOutput
        {
            constexpr gui::Length sms_radius                   = 8;
            constexpr gui::Length default_h                    = 30;
            constexpr gui::Length sms_max_width                = 320;
            constexpr gui::Length sms_h_padding                = 15;
            constexpr gui::Length sms_h_big_padding            = 25;
            constexpr gui::Length sms_v_padding                = 10;
            constexpr gui::Length sms_vertical_spacer          = 10;
            constexpr gui::Length sms_error_icon_left_margin   = 5;
            constexpr gui::Length sms_error_icon_right_margin  = 2;
            const inline gui::Padding sms_left_bubble_padding  = gui::Padding(smsOutput::sms_h_big_padding,
                                                                             smsOutput::sms_v_padding,
                                                                             smsOutput::sms_h_padding,
                                                                             smsOutput::sms_v_padding);
            const inline gui::Padding sms_right_bubble_padding = gui::Padding(smsOutput::sms_h_padding,
                                                                              smsOutput::sms_v_padding,
                                                                              smsOutput::sms_h_big_padding,
                                                                              smsOutput::sms_v_padding);
        } // namespace smsOutput

        namespace smsList
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = style::header::height;
            constexpr uint32_t h = style::window::default_body_height;
            constexpr uint32_t w = style::listview::body_width_with_scroll;
        } // namespace smsList

        namespace templates
        {
            namespace list
            {
                constexpr uint32_t x = style::window::default_left_margin;
                constexpr uint32_t y = style::header::height;
                constexpr uint32_t h = style::window_height - y - style::footer::height;
                constexpr uint32_t w = style::listview::body_width_with_scroll;

            } // namespace list
        }     // namespace templates
    }         // namespace messages
} // namespace style
