#pragma once
#include <Style.hpp>

namespace phonebookStyle
{
namespace search {
    constexpr uint32_t default_x = style::window::default_left_margin;
    constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
    namespace searchHeader {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 127;
        constexpr uint32_t w = 86;
        constexpr uint32_t h = 20;
    } // namespace searchHeader
    namespace horizontalBox {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 153;
        constexpr uint32_t w = default_w;
        constexpr uint32_t h = 33;
        constexpr uint32_t penWidth = 2;
        namespace searchTop {
            constexpr uint32_t x = default_w;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = 32;
            constexpr uint32_t h = 32;
        } // namespace searchTop
        namespace inputField {
            constexpr uint32_t x = 1;
            constexpr uint32_t y = 1;
            constexpr uint32_t w = default_w - searchTop::w;
            constexpr uint32_t h = 33;
        } // namespace inputField
    } // namespace horizontalBox
    namespace searchResultList {
        constexpr uint32_t x = 11;
        constexpr uint32_t y = 105;
        constexpr uint32_t w = 480 - 22;
        constexpr uint32_t h = 600 - 105 - 50;
        constexpr uint32_t maxElements = 7;
        constexpr uint32_t pageSize = 7;
        constexpr uint32_t penFocusWidth = 0;
        constexpr uint32_t penWidth = 0;
    } // namespace searchResultList
} //namespace search
} //namespace phonebookStyle