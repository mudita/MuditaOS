#pragma once

namespace style
{
    namespace messages
    {
        namespace threads
        {
            constexpr uint32_t pageSize = 5;

            constexpr uint32_t listPositionX = style::window::default_left_margin;
            // Magic 1 -> discussed with Design for proper alignment.
            constexpr uint32_t ListPositionY = style::header::height - 1;
            constexpr uint32_t listHeight    = style::window_height - ListPositionY - style::footer::height;
            constexpr uint32_t listWidth     = style::listview::body_width_with_scroll;
        } // namespace threads

        namespace threadItem
        {
            constexpr uint32_t topMargin    = 16;
            constexpr uint32_t bottomMargin = 13;

            constexpr uint32_t leftMargin  = 10;
            constexpr uint32_t rightMargin = 10;

            constexpr uint32_t timestampWidth    = 100;
            constexpr uint32_t cotactWidthOffset = timestampWidth + leftMargin + rightMargin;
            constexpr uint32_t notSentIconWidth  = 20;

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

        namespace templates
        {
            namespace list
            {
                constexpr uint32_t pageSize = 7;

                constexpr uint32_t x = style::window::default_left_margin;
                constexpr uint32_t y = style::header::height;
                constexpr uint32_t h = style::window_height - y - style::footer::height;
                constexpr uint32_t w = style::listview::body_width_with_scroll;

            } // namespace list
        }     // namespace templates
    }         // namespace messages
} // namespace style
