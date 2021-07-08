// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/SideListItem.hpp>
#include <widgets/Spinner.hpp>
#include <widgets/Style.hpp>

#include <string>

namespace gui
{
    class TimeSetSpinner : public SideListItem
    {
      public:
        TimeSetSpinner(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description = "");

        ~TimeSetSpinner() override = default;

        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;

      private:
        const uint32_t hourMin    = 0;
        const uint32_t hourMax    = 23;
        const uint32_t hourStep   = 1;
        const uint32_t minuteMin  = 0;
        const uint32_t minuteMax  = 59;
        const uint32_t minuteStep = 1;
        const uint32_t noOfDigits = 2;
        gui::Spinner *hour        = nullptr;
        gui::Spinner *minute      = nullptr;
        std::string fontName      = style::window::font::supersizemelight;

        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto onInput(const gui::InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getFontHeight() const noexcept -> uint16_t;
        [[nodiscard]] auto getWidestDigitWidth() const noexcept -> uint32_t;
    };

} /* namespace gui */
