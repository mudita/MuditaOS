// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"
#include "spinners/Spinners.hpp"
#include <gui/widgets/Style.hpp>
#include <gui/widgets/TextConstants.hpp>

#include <string>

namespace style::time_set_spinner
{
    namespace focus
    {
        inline constexpr auto size = 6U;
    } // namespace focus
    namespace colonIconSize
    {
        inline constexpr auto smallW  = 4U;
        inline constexpr auto smallH  = 21U;
        inline constexpr auto mediumW = 7U;
        inline constexpr auto mediumH = 26U;
        inline constexpr auto bigW    = 24U;
        inline constexpr auto bigH    = 84U;
    } // namespace colonIconSize
} // namespace style::time_set_spinner

namespace gui
{
    class ImageBox;

    class TimeSetSpinner : public HBox
    {
      public:
        enum class Size
        {
            SMALL,
            MEDIUM,
            BIG
        };

        struct ColonIconData
        {
            std::string iconName;
            Length w;
            Length h;
        };

        TimeSetSpinner(Item *parent, Size size, Length x, Length y, Length w, Length h);

        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        auto setFont(const std::string &newFontName) noexcept -> void;
        auto setFont(std::string newFocusFontName, std::string newNoFocusFontName) noexcept -> void;
        auto updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void;
        auto setEditMode(EditMode editMode) noexcept -> void;
        auto setHourRange(std::uint32_t min, std::uint32_t max) -> void;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;

      private:
        UIntegerSpinner *hour = nullptr;
        ImageBox *colon       = nullptr;
        const ColonIconData colonIconData{};
        UIntegerSpinnerFixed *minute = nullptr;
        EditMode editMode            = EditMode::Edit;
        Item *lastFocus              = nullptr;
        std::string focusFontName    = style::window::font::supersizeme;
        std::string noFocusFontName  = style::window::font::supersizemelight;

        void updateFocus(Item *newFocus);
        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getFontHeight(const std::string &fontName) const noexcept -> uint16_t;
        [[nodiscard]] auto getWidestDigitWidth(const std::string &fontName) const noexcept -> uint32_t;
        [[nodiscard]] auto getColonIconData(Size size) const noexcept -> ColonIconData;
    };
} /* namespace gui */
