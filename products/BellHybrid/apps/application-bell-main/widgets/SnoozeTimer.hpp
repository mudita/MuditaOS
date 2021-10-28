// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/TextFixedSize.hpp>
#include <gui/widgets/TextConstants.hpp>
#include <time/time_locale.hpp>
#include <string>

namespace gui
{
    class TimeSetSpinner;
    class ImageBox;

    class SnoozeTimer : public HBox
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

        explicit SnoozeTimer(Item *parent = nullptr, Position x = 0U, Position y = 0U, Length w = 0U, Length h = 0U);

        auto setFont(std::string newFontName) noexcept -> void;
        auto setTime(std::uint8_t mins, std::uint8_t secs) noexcept -> void;

      private:
        TimeSetSpinner *timeSpinner = nullptr;
        TextFixedSize *minusText    = nullptr;

        Status alarmStatus   = Status::DEACTIVATED;
        std::string fontName = style::window::font::largelight;
    };
} /* namespace gui */
