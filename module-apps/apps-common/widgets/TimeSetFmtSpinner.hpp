// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/TextConstants.hpp>
#include <time/time_locale.hpp>
#include <widgets/spinners/Spinners.hpp>
#include "TimeSetSpinner.hpp"

#include <string>

namespace style::time_set_fmt_spinner
{
    inline constexpr auto small_margin = 5U;
    inline constexpr auto big_margin   = 20U;
} // namespace style::time_set_fmt_spinner

namespace gui
{
    /// Time set spinner widget class with option for dynamic switching between 24/12-hour format
    /// Automatically recalculates hour upon switching format
    /// Can be used as a basic time displaying widget when @ref EditMode set to 'Browse'
    /// @ref KeyCode::KEY_ENTER switches hour -> minute -> time format
    /// @ref KeyCode::KEY_RF switches back
    /// Two time formats are supported:
    /// utils::time::Locale::TimeFormat::FormatTime12H
    /// utils::time::Locale::TimeFormat::FormatTime24H
    class TimeSetFmtSpinner : public HBox
    {
      public:
        explicit TimeSetFmtSpinner(
            Item *parent                               = nullptr,
            uint32_t x                                 = 0U,
            uint32_t y                                 = 0U,
            uint32_t w                                 = 0U,
            uint32_t h                                 = 0U,
            utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H);

        /// Switches currently displayed time format
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void;
        auto setTimeFormatSpinnerVisibility(bool visibility) noexcept -> void;
        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        auto setTime(std::time_t time) noexcept -> void;
        auto setEditMode(EditMode newEditMode) noexcept -> void;
        auto setFont(std::string newFontName) noexcept -> void;
        auto setFont(std::string newFocusFontName, std::string newNoFocusFontName) noexcept -> void;
        auto updateFmtFont(const std::string &fontName) noexcept -> void;

        [[nodiscard]] auto getTime() const noexcept -> std::time_t;
        [[nodiscard]] auto getHour() const noexcept -> int;
        [[nodiscard]] auto getMinute() const noexcept -> int;
        [[nodiscard]] auto isPM() const noexcept -> bool;

      private:
        enum class TraverseDir : bool
        {
            Left,
            Right
        };

        std::map<std::string, Margins> fmtMarginsMap = {
            {style::window::font::largelight, {style::time_set_fmt_spinner::small_margin, 0, 0, 0}},
            {style::window::font::supersizemelight, {style::time_set_fmt_spinner::big_margin, 0, 0, 0}},
            {style::window::font::huge, {style::time_set_fmt_spinner::big_margin, 0, 0, 0}}};
        [[nodiscard]] auto getFmtMargins(const std::string &fmtFont) const noexcept -> Margins;

        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        void handleContentChanged() override;

        TimeSetSpinner *timeSetSpinner = nullptr;
        UTF8Spinner *fmt               = nullptr;
        EditMode editMode              = EditMode::Edit;
        std::string focusFontName      = style::window::font::supersizeme;
        std::string noFocusFontName    = style::window::font::supersizemelight;

        utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H;
    };

} // namespace gui
