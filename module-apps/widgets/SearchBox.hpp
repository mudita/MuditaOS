#pragma once

#include <Text.hpp>
#include <BoxLayout.hpp>
#include "i18/i18.hpp"
#include <Image.hpp>

namespace phonebookStyle::search
{
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
} // namespace phonebookStyle::search

namespace gui
{

    gui::Text *searchBox(gui::Item *parent, const std::string &header, const std::string &icon);
}; // namespace gui
