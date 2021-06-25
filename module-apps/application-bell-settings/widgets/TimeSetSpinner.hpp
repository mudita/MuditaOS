// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/SideListItem.hpp>
#include <module-gui/gui/widgets/Spinner.hpp>

namespace gui
{
    class TimeSetSpinner : public SideListItem
    {
      public:
        TimeSetSpinner(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description = "");

        ~TimeSetSpinner() override = default;

        auto setHour(int value) const noexcept -> void;
        auto setMinute(int value) const noexcept -> void;
        auto getHour() const noexcept -> int;
        auto getMinute() const noexcept -> int;

      private:
        const uint32_t hourMin    = 0;
        const uint32_t hourMax    = 23;
        const uint32_t hourStep   = 1;
        const uint32_t minuteMin  = 0;
        const uint32_t minuteMax  = 59;
        const uint32_t minuteStep = 1;
        gui::Spinner *hour        = nullptr;
        gui::Spinner *minute      = nullptr;

        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto onInput(const gui::InputEvent &inputEvent) -> bool override;
    };

} /* namespace gui */
