// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <module-gui/gui/widgets/Image.hpp>

namespace style::window::modes
{
    constexpr inline auto invision_diff = 26;

    constexpr inline auto left_offset   = style::window_width / 4;
    constexpr inline auto top_offset    = 182;
    constexpr inline auto bottom_offset = 257;
    constexpr inline auto height        = style::window_height - top_offset - bottom_offset;
    constexpr inline auto width         = style::window_width - left_offset;

    namespace connected
    {
        constexpr inline auto title_key = "home_modes_connected";
        constexpr inline auto width     = 129;
        constexpr inline auto height    = 33;

        namespace margin
        {
            constexpr inline auto left  = 296 - style::window::modes::left_offset + invision_diff;
            constexpr inline auto right = 20;
        } // namespace margin
    }     // namespace connected

    namespace notdisturb
    {
        constexpr inline auto title_key = "home_modes_notdisturb";
        constexpr inline auto width     = 297 + invision_diff;
        constexpr inline auto height    = 51;

        namespace margin
        {
            constexpr inline auto top    = 20;
            constexpr inline auto left   = 128 - style::window::modes::left_offset;
            constexpr inline auto bottom = 20;
            constexpr inline auto right  = 0;
        } // namespace margin
    }     // namespace notdisturb

    namespace offline
    {
        constexpr inline auto title_key = "home_modes_offline";
        constexpr inline auto width     = 88;
        constexpr inline auto height    = 33;

        namespace margin
        {
            constexpr inline auto left  = 337 - style::window::modes::left_offset + invision_diff;
            constexpr inline auto right = 10;
        } // namespace margin
    }     // namespace offline

    namespace image
    {
        namespace margin
        {
            constexpr inline auto left = 7;
        }
        constexpr inline auto width  = 10;
        constexpr inline auto height = 20;
    } // namespace image

} // namespace style::window::modes

namespace gui
{
    class ModeRow : public HBox
    {
        Label *label = nullptr;
        Image *img   = nullptr;

      public:
        ModeRow(Item *parent = nullptr, uint32_t x = 0, uint32_t y = 0, uint32_t width = 0, uint32_t height = 0);

        void addText(const std::string &text,
                     const std::string &fontSize,
                     const Margins &margin,
                     uint32_t width,
                     uint32_t height);

        void addImage(const std::string &imageName);
    };

    class ModesBox : public VBox
    {
        ModeRow *connected  = nullptr;
        ModeRow *notDisturb = nullptr;
        ModeRow *offline    = nullptr;

        void addConnected();

        void addNotDisturb();

        void addOffline();

      public:
        ModesBox(Item *parent = nullptr, uint32_t x = 0, uint32_t y = 0);
    };
} // namespace gui
