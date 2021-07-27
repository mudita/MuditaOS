// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>

namespace musicPlayerStyle
{
    namespace mainWindow
    {
        namespace songTitle
        {
            constexpr uint32_t x = 40;
            constexpr uint32_t y = 110;
            constexpr uint32_t w = 400;
            constexpr uint32_t h = 35;
        } // namespace songTitle
        namespace songAuthor
        {
            constexpr uint32_t x = 40;
            constexpr uint32_t y = 161;
            constexpr uint32_t w = 400;
            constexpr uint32_t h = 35;
        } // namespace songAuthor
        namespace songProgress
        {
            constexpr uint32_t x = 31;
            constexpr uint32_t y = 232;
            constexpr uint32_t w = 419;
            constexpr uint32_t h = 12;
        } // namespace songProgress
        namespace songCurrentTime
        {
            constexpr uint32_t x = 30;
            constexpr uint32_t y = 270;
            constexpr uint32_t w = 100;
            constexpr uint32_t h = 20;
        } // namespace songCurrentTime
        namespace songDuration
        {
            constexpr uint32_t x = 350;
            constexpr uint32_t y = 278;
            constexpr uint32_t w = 100;
            constexpr uint32_t h = 20;
        } // namespace songDuration
        namespace rewind
        {
            constexpr uint32_t x = 84;
            constexpr uint32_t y = 342;
        } // namespace rewind
        namespace fastForward
        {
            constexpr uint32_t x = 364;
            constexpr uint32_t y = 342;
        } // namespace fastForward
        namespace action
        {
            constexpr uint32_t x = 224;
            constexpr uint32_t y = 342;
        } // namespace action
        namespace musicLibrary
        {
            constexpr uint32_t x = 140;
            constexpr uint32_t y = 456;
            constexpr uint32_t w = 201;
            constexpr uint32_t h = 20;
        } // namespace musicLibrary
    }     // namespace mainWindow
    namespace allSongsWindow
    {
        constexpr uint32_t x = style::window::default_left_margin;
        // Magic 1 -> discussed with Design for proper alignment.
        constexpr uint32_t y = style::window::default_vertical_pos - 1;
        constexpr uint32_t w = style::listview::body_width_with_scroll;
        // Bottom margin need to be added to fit all elements.
        constexpr uint32_t h = style::window_height - y - style::footer::height + style::margins::small;
    } // namespace allSongsWindow
    namespace emptyWindow
    {
        namespace noteImg
        {
            constexpr uint32_t x = 176;
            constexpr uint32_t y = 102;
        } // namespace noteImg
        namespace placeHolderImg
        {
            constexpr uint32_t x = 80;
            constexpr uint32_t y = 374;
        } // namespace placeHolderImg
        namespace infoText
        {
            constexpr uint32_t x = 40;
            constexpr uint32_t y = 260;
            constexpr uint32_t w = 400;
            constexpr uint32_t h = 66;
        } // namespace infoText
    }     // namespace emptyWindow

    namespace songItem
    {
        constexpr uint32_t w = style::window::default_body_width;
        constexpr uint32_t h = 100;

        constexpr uint32_t bold_text_h = 33;
        constexpr uint32_t text_h      = 33;
        constexpr uint32_t duration_w  = 50;

        constexpr int32_t topMargin   = 16;
        constexpr int32_t leftMargin  = 10;
        constexpr int32_t rightMargin = 4;

    } // namespace songItem

    namespace volumeLabel
    {
        constexpr uint32_t x = 0;
        constexpr uint32_t y = 52;

        constexpr uint32_t w = style::window_width;
        constexpr uint32_t h = 52;

        constexpr auto defaultVolumeLabelText = "Vol: 10";

        constexpr int32_t topMargin    = 0;
        constexpr int32_t bottomMargin = 0;

        constexpr int32_t leftMargin  = 16;
        constexpr int32_t rightMargin = 0;
    } // namespace volumeLabel

} // namespace musicPlayerStyle
