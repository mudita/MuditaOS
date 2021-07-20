// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"
#include <gui/widgets/Spinner.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/TextConstants.hpp>

#include <string>

namespace style
{
    namespace time_set_spinner
    {
        namespace colon
        {
            inline constexpr auto w            = 32U;
            inline constexpr auto h            = 100U;
            inline constexpr auto default_font = style::window::font::supersizemelight;
        } // namespace colon
    }     // namespace time_set_spinner
} // namespace style

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
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;

      private:
        Spinner *hour        = nullptr;
        Label *colon         = nullptr;
        Spinner *minute      = nullptr;
        EditMode editMode    = EditMode::Edit;
        std::string fontName = style::window::font::supersizemelight;

        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getFontHeight() const noexcept -> uint16_t;
        [[nodiscard]] auto getWidestDigitWidth() const noexcept -> uint32_t;
    };
} /* namespace gui */
