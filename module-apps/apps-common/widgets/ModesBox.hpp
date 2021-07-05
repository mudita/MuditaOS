// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <utility>

namespace sys::phone_modes
{
    enum class PhoneMode;
} // namespace sys::phone_modes

namespace style::window::modes
{
    constexpr inline auto top_offset        = 182;
    constexpr inline auto bottom_offset     = 235;
    constexpr inline auto height            = style::window_height - top_offset - bottom_offset;
    constexpr inline auto width             = style::window_width;
    constexpr inline auto number_of_entries = 4;
    namespace connected
    {
        constexpr inline auto title_key = "home_modes_connected";

    } // namespace connected

    namespace notdisturb
    {
        constexpr inline auto title_key = "home_modes_notdisturb";

    } // namespace notdisturb

    namespace offline
    {
        constexpr inline auto title_key       = "home_modes_offline";
        constexpr inline auto description_key = "home_modes_message_only";
    } // namespace offline

    namespace image
    {
        constexpr inline auto width = 55;
    } // namespace image

    namespace text
    {
        constexpr inline auto width = style::window::modes::width - style::window::modes::image::width;
    } // namespace text
} // namespace style::window::modes

namespace gui
{
    class ImageBox;

    class ModeRow : public HBox
    {
        Label *label  = nullptr;
        ImageBox *img = nullptr;

      public:
        ModeRow(Item *parent = nullptr, uint32_t x = 0, uint32_t y = 0, uint32_t width = 0, uint32_t height = 0);

        void addText(const std::string &text, const std::string &fontSize, uint32_t width, uint32_t height);

        void update(std::pair<std::string, const bool> &&params);
        void addImage(const std::string &imageName);
    };

    class ModesBox : public VBox
    {
        ModeRow *connected   = nullptr;
        ModeRow *notDisturb  = nullptr;
        ModeRow *offline     = nullptr;
        ModeRow *messageOnly = nullptr;

        void addConnected();
        void addNotDisturb();
        void addOffline();

      public:
        ModesBox(Item *parent = nullptr, uint32_t x = 0, uint32_t y = 0);
        void update(const sys::phone_modes::PhoneMode &phoneMode, const bool flightMode);
    };
} // namespace gui
