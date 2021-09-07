// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"
#include <gui/widgets/Spinner.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/TextConstants.hpp>

#include <string>

namespace style::time_set_spinner
{
    namespace focus
    {
        inline constexpr auto size = 3U;
    } // namespace focus
} // namespace style::time_set_spinner

namespace gui
{
    class TimeSetSpinner : public HBox
    {
      public:
        TimeSetSpinner(Item *parent, Length x, Length y, Length w, Length h);

        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        auto setFont(std::string newFontName) noexcept -> void;
        auto setEditMode(EditMode editMode) noexcept -> void;
        auto setHourMax(std::uint32_t newMax) noexcept -> void;
        auto setHourMin(std::uint32_t newMin) noexcept -> void;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;

        auto getMinimumSize() const noexcept -> std::pair<std::uint32_t, std::uint32_t>;

      private:
        Spinner *hour        = nullptr;
        Label *colon         = nullptr;
        Spinner *minute      = nullptr;
        EditMode editMode    = EditMode::Edit;
        Item *lastFocus      = nullptr;
        std::string fontName = style::window::font::supersizemelight;

        void updateFocus(Item *newFocus);
        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getFontHeight() const noexcept -> uint16_t;
        [[nodiscard]] auto getWidestDigitWidth() const noexcept -> uint32_t;
        [[nodiscard]] auto getColonWidth() const noexcept -> uint32_t;
    };
} /* namespace gui */
