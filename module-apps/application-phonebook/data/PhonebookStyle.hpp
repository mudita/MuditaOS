#pragma once
#include <Style.hpp>

namespace phonebookStyle
{
namespace mainWindow {
    constexpr uint32_t default_x = style::window::default_left_margin;
    constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
    namespace leftArrowImage {
        constexpr uint32_t x = default_x;
        constexpr uint32_t y = 62;
        constexpr uint32_t w = 11;
        constexpr uint32_t h = 13;
    } // namespace leftArrowImage
    namespace rightArrowImage {
        constexpr uint32_t x = style::window_width - style::window::default_left_margin - 11;
        constexpr uint32_t y = 62;
        constexpr uint32_t w = 11;
        constexpr uint32_t h = 13;
    } // namespace rightArrowImage
    namespace newContactImage {
        constexpr uint32_t x = style::window::default_left_margin + 20;
        constexpr uint32_t y = 55;
        constexpr uint32_t w = 24;
        constexpr uint32_t h = 24;
    } // namespace newContactImage
    namespace searchImage {
        constexpr uint32_t x = style::window_width - style::window::default_left_margin - rightArrowImage::w - 8 - 26;
        constexpr uint32_t y = 55;
        constexpr uint32_t w = 26;
        constexpr uint32_t h = 26;
    } // namespace searchImage
    namespace contactsList {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 105 + 6;
        constexpr uint32_t w = style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        constexpr uint32_t h = style::window_height - y - 50;
        constexpr uint32_t maxElements = 7;
        constexpr uint32_t pageSize = 7;
        constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
        constexpr uint32_t penWidth = 0;
    } // namespace contactsList
} // namespace mainWindow
namespace search {
    constexpr uint32_t default_x = style::window::default_left_margin;
    constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        namespace searchHeader
        {
            constexpr uint32_t x = style::window::default_left_margin;
            constexpr uint32_t y = 127;
            constexpr uint32_t w = 86;
            constexpr uint32_t h = 20;
        } // namespace searchHeader
        namespace horizontalBox
        {
            constexpr uint32_t x        = style::window::default_left_margin;
            constexpr uint32_t y        = 153;
            constexpr uint32_t w        = default_w;
            constexpr uint32_t h        = 33;
            constexpr uint32_t penWidth = 2;
            namespace searchTop
            {
                constexpr uint32_t x = default_w;
                constexpr uint32_t y = 1;
                constexpr uint32_t w = 32;
                constexpr uint32_t h = 32;
            } // namespace searchTop
            namespace inputField
            {
                constexpr uint32_t x = 1;
                constexpr uint32_t y = 1;
                constexpr uint32_t w = default_w - searchTop::w;
                constexpr uint32_t h = 33;
            } // namespace inputField
        }     // namespace horizontalBox
    }         // namespace search
    namespace searchResults
    {
        constexpr uint32_t default_x = style::window::default_left_margin;
        constexpr uint32_t default_w =
            style::window_width - style::window::default_left_margin - style::window::default_right_margin;
    namespace searchResultList {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = 105 + 6;
        constexpr uint32_t w = style::window_width - style::window::default_left_margin - style::window::default_right_margin;
        constexpr uint32_t h = style::window_height - y - 50;
        constexpr uint32_t maxElements = 7;
        constexpr uint32_t pageSize = 7;
        constexpr uint32_t penFocusWidth = style::window::default_border_no_focus_w;
        constexpr uint32_t penWidth = 0;
    } // namespace searchResultList
} //namespace searchResults
} //namespace phonebookStyle
