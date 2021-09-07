// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Style.hpp>

namespace style::meditation
{
    constexpr auto VerticalWidgetGap = 20;
    namespace body
    {
        constexpr auto Width  = style::window_width;
        constexpr auto Height = style::window_height - style::window::default_vertical_pos - style::footer::height - 1;
        constexpr auto X      = 1;
        constexpr auto Y      = style::window::default_vertical_pos;
    } // namespace body

    namespace timer
    {
        constexpr auto Radius = 150;
        constexpr auto X      = body::X + (body::Width / 2) - Radius;
        constexpr auto Y      = body::Y + 20;
        constexpr auto Width  = 2 * Radius;
        constexpr auto Height = Width;

        constexpr auto BorderColor = gui::ColorGrey;
        constexpr auto BorderColorOnFocused = gui::ColorFullBlack;
        constexpr auto PenWidth    = 3;

        namespace setterValueLabel
        {
            constexpr auto X      = 100;
            constexpr auto Y      = 100;
            constexpr auto Width  = timer::Width - 2 * X;
            constexpr auto Height = timer::Height - 2 * Y;
        } // namespace setterValueLabel

        namespace setterUnitLabel
        {
            constexpr auto X      = 0;
            constexpr auto Y      = setterValueLabel::Y + setterValueLabel::Height;
            constexpr auto Width  = timer::Width;
            constexpr auto Height = 40;
        } // namespace setterUnitLabel

        namespace infoText
        {
            constexpr auto X      = style::window::default_left_margin;
            constexpr auto Y      = 0;
            constexpr auto Width  = style::window::default_body_width;
            constexpr auto Height = style::window_height;
        } // namespace infoText

    } // namespace timer

    namespace intervalBox
    {
        constexpr auto X      = style::window::default_left_margin;
        constexpr auto Y      = style::meditation::timer::Y + style::meditation::timer::Height + VerticalWidgetGap;
        constexpr auto Width  = style::window::default_body_width;
        constexpr auto Height = 60;

        namespace topLabel
        {
            constexpr auto X      = 0;
            constexpr auto Y      = 0;
            constexpr auto Width  = intervalBox::Width;
            constexpr auto Height = intervalBox::Height / 2;
        } // namespace topLabel

        namespace bottomLabel
        {
            constexpr auto X      = 0;
            constexpr auto Y      = topLabel::Y + topLabel::Height;
            constexpr auto Width  = intervalBox::Width;
            constexpr auto Height = intervalBox::Height / 2;
        } // namespace bottomLabel

        namespace arrow
        {
            constexpr auto Y      = 10;
            constexpr auto Width  = 15;
            constexpr auto Height = intervalBox::Height / 2;
            constexpr auto LeftX  = 0;
            constexpr auto RightX = intervalBox::Width - arrow::Width;
        } // namespace arrow

    } // namespace intervalBox

    namespace itemList
    {
        namespace text
        {
            constexpr auto X      = 10;
            constexpr auto Y      = 0;
            constexpr auto Width  = style::window::default_body_width;
            constexpr auto Height = style::window::label::big_h;
        } // namespace text

        namespace image
        {
            constexpr auto X      = style::window::default_body_width - 50;
            constexpr auto Y      = 12;
            constexpr auto OnOffY = 15;
            constexpr auto Width  = 0;
            constexpr auto Height = 0;
        } // namespace image

    } // namespace itemList

    namespace listView
    {
        namespace window
        {
            constexpr auto X      = style::window::default_left_margin;
            constexpr auto Y      = style::window::default_vertical_pos;
            constexpr auto Width  = style::listview::body_width_with_scroll;
            constexpr auto Height = style::window_height - Y - style::footer::height;
        } // namespace window

    } // namespace listView

} // namespace style::meditation
