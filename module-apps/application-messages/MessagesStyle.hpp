/*
 * MessagesStyle.hpp
 *
 *  Created on: 25 lis 2019
 *      Author: kuba
 */

#pragma once

namespace messages
{
    namespace threads
    {
        constexpr auto pageSize = 5;

        constexpr auto listHeight    = 445;
        constexpr auto listWidth     = 458;
        constexpr auto listPositionX = 11;
        constexpr auto ListPositionY = 105; // @TODO M.G. tweak it to design. Spacers between entries, top margin
    }                                       // namespace threads

    namespace threadItem
    {

        constexpr auto contactPositionX  = 14;
        constexpr auto contactPositionY  = 10;
        constexpr auto cotactWidthOffset = 133;

        constexpr auto timestampWidth = 100;

        constexpr auto prewievPositionX   = 14;
        constexpr auto previewWidthOffset = 20;
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
} // namespace messages
