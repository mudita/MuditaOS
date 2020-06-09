#pragma once

namespace style
{
    namespace messages
    {
        namespace threads
        {
            constexpr auto pageSize = 5;

            constexpr auto listPositionX = style::window::default_left_margin;
            constexpr auto ListPositionY = style::header::height + style::listview::top_margin_small;
            constexpr auto listHeight    = style::window_height - ListPositionY - style::footer::height;
            constexpr auto listWidth     = style::listview::body_width_with_scroll;
        } // namespace threads

        namespace threadItem
        {
            constexpr auto topMargin    = 16;
            constexpr auto bottomMargin = 13;

            constexpr auto leftMargin  = 10;
            constexpr auto rightMargin = 10;

            constexpr auto cotactWidthOffset = 120;
            constexpr auto timestampWidth = 100;
            constexpr auto notSentIconWidth  = 20;

            constexpr auto previewWidthOffset = leftMargin + rightMargin + 10;
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
                constexpr auto pageSize = 7;

                constexpr auto x = style::window::default_left_margin;
                constexpr auto y = style::header::height + style::listview::top_margin_small;
                constexpr auto h = style::window_height - y - style::footer::height;
                constexpr auto w = style::listview::body_width_with_scroll;

            } // namespace list
        }     // namespace templates
    }         // namespace messages
} // namespace style
