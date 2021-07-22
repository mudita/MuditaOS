// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Rect.hpp"
#include <application-bell-main/data/BellAlarmData.hpp>
#include "TimeSetSpinner.hpp"
#include <gui/widgets/Spinner.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/TextConstants.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <string>

namespace style::alarm_set_spinner
{
    namespace arrow
    {
        inline constexpr auto w = 64U;
        inline constexpr auto h = 64U;
    } // namespace arrow
    inline constexpr auto w = 440U;
    inline constexpr auto h = 64U;

} // namespace style::alarm_set_spinner

namespace gui
{
    class AlarmSetSpinner : public HBox
    {
      public:
        AlarmSetSpinner(Item *parent, Length x, Length y, Length w, Length h);

        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        auto setFont(std::string newFontName) noexcept -> void;
        auto setEditMode(EditMode mode) noexcept -> void;
        auto setAlarmStatus(BellAlarm::Status status) noexcept -> void;
        auto getAlarmStatus() noexcept -> BellAlarm::Status;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;

      private:
        TimeSetSpinner *timeSpinner = nullptr;
        ImageBox *leftArrow         = nullptr;
        ImageBox *rightArrow        = nullptr;
        ImageBox *alarmImg          = nullptr;

        EditMode editMode             = EditMode::Edit;
        BellAlarm::Status alarmStatus = BellAlarm::Status::DEACTIVATED;
        std::string fontName          = style::window::font::largelight;
        auto onInput(const InputEvent &inputEvent) -> bool override;
    };
} /* namespace gui */
