// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/spinners/Spinners.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/text/TextConstants.hpp>
#include <time/time_locale.hpp>

#include <string>

namespace style::time_set_spinner_vertical
{
    namespace focus
    {
        inline constexpr auto size = 6U;
    } // namespace focus

    inline constexpr auto small_margin = 6U;
    inline constexpr auto big_margin   = 44U;
    inline constexpr auto colossal_margin = -40;
} // namespace style::time_set_spinner_vertical

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
    class TimeSetSpinnerVertical : public VBox
    {
      public:
        TimeSetSpinnerVertical(
            Item *parent                               = nullptr,
            uint32_t x                                 = 0U,
            uint32_t y                                 = 0U,
            uint32_t w                                 = 0U,
            uint32_t h                                 = 0U,
            utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H);

        /// Switches currently displayed time format
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void;
        // auto setHour(int value) noexcept -> void;
        // auto setMinute(int value) noexcept -> void;
        auto setTime(std::time_t time) noexcept -> void;
        auto setFont(const std::string &newFontName) noexcept -> void;
        auto updateFont(TextFixedSize *elem, const std::string &fontName) noexcept -> void;

        //[[nodiscard]] auto getTime() const noexcept -> std::time_t;
        // [[nodiscard]] auto getHour() const noexcept -> int;
        // [[nodiscard]] auto getMinute() const noexcept -> int;
        [[nodiscard]] auto isPM() const noexcept -> bool;

        auto getTimeFormat() const noexcept -> utils::time::Locale::TimeFormat;

        auto getHourMargins(const std::string &font) const noexcept -> Margins;

      private:
        std::map<std::string, Margins> spaceMarginsMap = {
            {style::window::font::verybiglight, {0, 0, 0, style::time_set_spinner_vertical::small_margin}},
            {style::window::font::largelight, {0, 0, 0, style::time_set_spinner_vertical::small_margin}},
            {style::window::font::supersizemelight, {0, 0, 0, style::time_set_spinner_vertical::big_margin}},
            {style::window::font::huge, {0, 0, 0, style::time_set_spinner_vertical::big_margin}},
            {style::window::font::colossal, {0, 0, 0, style::time_set_spinner_vertical::colossal_margin}}};

        void handleContentChanged() override;

        gui::HBox *hBoxHours              = nullptr;
        gui::HBox *hBoxMinutes              = nullptr;
        gui::TextFixedSize *hour_first   = nullptr;
        gui::TextFixedSize *hour_second   = nullptr;
        gui::TextFixedSize *minute_first   = nullptr;
        gui::TextFixedSize *minute_second   = nullptr;
        std::string fontName         = style::window::font::supersizemelight;

        utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H;
    };
} /* namespace gui */
