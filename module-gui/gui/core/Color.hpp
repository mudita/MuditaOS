/*
 * Color.hpp
 *
 *  Created on: 24 kwi 2019
 *      Author: robert
 */

#ifndef GUI_CORE_COLOR_HPP_
#define GUI_CORE_COLOR_HPP_

#include <cstdint>

namespace gui
{

    class Color
    {
      public:
        Color() : intensivity{0}, alpha{0} {};
        Color(uint8_t intensivity, uint8_t alpha) : intensivity{intensivity}, alpha{alpha} {};
        // this defines color of the pixel.
        uint8_t intensivity;
        // defines how transparent is pixel. 0 means solid color and GUI_COLORS_COUNT-1 fully transparent
        uint8_t alpha;
    };

    extern Color ColorFullBlack;
    extern Color ColorFullWhite;
    extern Color ColorNoColor;
    extern Color ColorTray;

} // namespace gui

#endif /* GUI_CORE_COLOR_HPP_ */
