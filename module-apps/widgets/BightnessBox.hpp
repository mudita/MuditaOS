// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <module-gui/gui/widgets/Image.hpp>

namespace style::window::brightness
{
    const inline auto title_key = "brightness_text";

    namespace bar
    {
        constexpr inline auto brightness_levels = 5;
        constexpr inline auto left_offset       = 160;
        constexpr inline auto top_offset        = 520;
    } // namespace bar

    namespace box
    {
        constexpr inline auto top_offset = 405;
        constexpr inline auto height     = style::window_height - top_offset;
        constexpr inline auto width      = style::window_width;
    } // namespace box

    namespace title
    {
        constexpr inline auto width  = 213;
        constexpr inline auto height = 33;

        constexpr inline auto space_width  = 20;
        constexpr inline auto arrow_width  = 20;
        constexpr inline auto arrow_height = 20;

        constexpr inline auto label_width  = 135;
        constexpr inline auto label_height = 33;
        constexpr inline auto top_offset   = 463;
        constexpr inline auto left_offset  = 134;
    } // namespace title
} // namespace style::window::brightness

namespace gui
{
    class BrightnessBox : public HBox
    {
        void addArrow(const std::string &arrowName, Alignment::Horizontal aligment);

        void addBrightnessTitle(Item *parent, const std::string &text);

      public:
        BrightnessBox(Item *parent = nullptr, uint32_t x = 0, uint32_t y = 0);
    };
} // namespace gui
