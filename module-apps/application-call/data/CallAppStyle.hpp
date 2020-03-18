#pragma once

#include <Style.hpp>

namespace callAppStyle
{
    // ENTER NUMBER WINDOW
    namespace enterNumberWindow
    {
        namespace numberLabel
        {
            constexpr uint32_t x       = 60;
            constexpr uint32_t y       = 157;
            constexpr uint32_t w       = style::window_width - 2 * x;
            constexpr uint32_t h       = 66;
            constexpr uint32_t borderW = 1;
        } // namespace numberLabel
        namespace newContactIcon
        {
            constexpr uint32_t x = 190;
            constexpr uint32_t y = 411;
        } // namespace newContactIcon
    }     // namespace enterNumberWindow

    namespace callWindow
    {
        namespace sendMessageIcon
        {
            constexpr uint32_t x = 190;
            constexpr uint32_t y = 411;
        } // namespace sendMessageIcon
    }     // namespace callWindow
} // namespace callAppStyle