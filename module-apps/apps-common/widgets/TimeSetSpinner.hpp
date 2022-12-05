// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/spinners/Spinners.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/text/TextConstants.hpp>

#include <string>

namespace style::time_set_spinner
{
    namespace focus
    {
        inline constexpr auto size = 6U;
    } // namespace focus

    inline constexpr auto very_small_margin       = 3U;
    inline constexpr auto small_margin = 6U;
    inline constexpr auto big_margin   = 6U;
    inline constexpr auto gargantuan_margin_left  = 14U;
    inline constexpr auto gargantuan_margin_right = 7U;
} // namespace style::time_set_spinner

namespace gui
{
    class ImageBox;

    class TimeSetSpinner : public HBox
    {
      public:
        explicit TimeSetSpinner(Item *parent);
        TimeSetSpinner(Item *parent, Length x, Length y, Length w, Length h);

        auto setTime(std::time_t time) noexcept -> void;
        auto setTime(int hourValue, int minuteValue) noexcept -> void;
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
        std::map<std::string, std::string> colonFontMap = {
            {style::window::font::verybiglight, "alarm_colon_W_M"},
            {style::window::font::veryverybiglight, "alarm_colon_W_M"},
            {style::window::font::largelight, "alarm_colon_W_M"},
            {style::window::font::supersizeme, "alarm_colon_select_W_M"},
            {style::window::font::supersizemelight, "alarm_colon_select_W_M"},
            {style::window::font::huge, "alarm_colon_clock_W_M"},
            {style::window::font::gargantuan, "alarm_colon_clock_W_M"}};

        std::map<std::string, Margins> colonMarginsMap = {
            {style::window::font::verybiglight,
             {style::time_set_spinner::very_small_margin, 0, style::time_set_spinner::very_small_margin, 0}},
            {style::window::font::veryverybiglight,
             {style::time_set_spinner::very_small_margin, 0, style::time_set_spinner::very_small_margin, 0}},
            {style::window::font::largelight,
             {style::time_set_spinner::small_margin, 0, style::time_set_spinner::small_margin, 0}},
            {style::window::font::supersizeme,
             {style::time_set_spinner::big_margin, 0, style::time_set_spinner::big_margin, 0}},
            {style::window::font::supersizemelight,
             {style::time_set_spinner::big_margin, 0, style::time_set_spinner::big_margin, 0}},
            {style::window::font::huge,
             {style::time_set_spinner::big_margin, 0, style::time_set_spinner::big_margin, 0}},
            {style::window::font::gargantuan,
             {style::time_set_spinner::gargantuan_margin_left,
              0,
              style::time_set_spinner::gargantuan_margin_right,
              0}}};

        U8IntegerSpinner *hour        = nullptr;
        ImageBox *colon              = nullptr;
        U8IntegerSpinnerFixed *minute = nullptr;
        EditMode editMode            = EditMode::Edit;
        Item *lastFocus              = nullptr;
        std::string focusFontName    = style::window::font::supersizeme;
        std::string noFocusFontName  = style::window::font::supersizemelight;

        void updateFocus(Item *newFocus);
        auto handleEnterKey() -> bool;
        auto handleRightFunctionKey() -> bool;
        auto applySizeRestrictions() -> void;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        [[nodiscard]] auto getFontHeight(const std::string &fontName) const noexcept -> uint16_t;
        [[nodiscard]] auto getColonImage(const std::string &colonFont) const noexcept -> std::string;
        [[nodiscard]] auto getColonMargins(const std::string &colonFont) const noexcept -> Margins;
    };
} /* namespace gui */
