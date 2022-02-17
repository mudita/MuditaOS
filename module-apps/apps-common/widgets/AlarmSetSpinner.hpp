// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/text/TextConstants.hpp>
#include <time/time_locale.hpp>
#include <string>

namespace style::alarm_set_spinner
{
    inline constexpr auto margin = 10U;
} // namespace style::alarm_set_spinner

namespace gui
{
    class TimeSetFmtSpinner;
    class ImageBox;

    class AlarmSetSpinner : public HBox
    {
      public:
        enum class Status
        {
            UNKNOWN,
            RINGING,
            ACTIVATED,
            DEACTIVATED,
            SNOOZE
        };

        explicit AlarmSetSpinner(
            Item *parent = nullptr, Position x = 0U, Position y = 0U, Length w = 0U, Length h = 0U);

        auto setArrowsVisible(bool visible) noexcept -> void;
        auto setTime(std::time_t time) noexcept -> void;
        auto setFont(std::string newFontName) noexcept -> void;
        auto setEditMode(EditMode mode) noexcept -> void;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void;
        auto setAlarmStatus(Status status) noexcept -> void;
        auto setAlarmTimeVisible(bool value) noexcept -> void;
        auto setMinute(std::uint8_t minute) noexcept -> void;
        auto getAlarmStatus() const noexcept -> Status;
        auto getTime() const noexcept -> std::time_t;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;
        [[nodiscard]] auto isPM() const noexcept -> bool;

      private:
        TimeSetFmtSpinner *timeSpinner = nullptr;
        ImageBox *leftArrow            = nullptr;
        ImageBox *rightArrow           = nullptr;
        ImageBox *alarmImg             = nullptr;

        EditMode editMode    = EditMode::Edit;
        Status alarmStatus   = Status::DEACTIVATED;
        std::string fontName = style::window::font::largelight;
        auto onInput(const InputEvent &inputEvent) -> bool override;
    };
} /* namespace gui */
