// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/TimeSetFmtSpinner.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/text/TextConstants.hpp>
#include <time/time_locale.hpp>
#include <string>

namespace gui
{
    class TimeSetSpinner;
    class ImageBox;

    class SnoozeTimer : public HBox
    {
      public:
        explicit SnoozeTimer(Item *parent = nullptr, Position x = 0U, Position y = 0U, Length w = 0U, Length h = 0U);

        auto setFont(std::string newFontName) noexcept -> void;
        auto setTime(std::uint8_t mins, std::uint8_t secs) noexcept -> void;
        auto setTime(std::time_t time) noexcept -> void;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void;
        auto setTimerVisible(bool visible) noexcept -> void;

      private:
        TimeSetFmtSpinner *timeSpinner = nullptr;

        std::string fontName = style::window::font::largelight;
    };
} /* namespace gui */
