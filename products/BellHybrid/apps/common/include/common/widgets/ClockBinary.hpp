// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui
{
    class BinaryDigitDisplay;

    class ClockBinary : public HBox
    {
      public:
        explicit ClockBinary(
            Item *parent = nullptr, std::uint32_t x = 0, std::uint32_t y = 0, std::uint32_t w = 0, std::uint32_t h = 0);

        auto setHour(int value) noexcept -> void;
        auto setMinute(int value) noexcept -> void;
        auto setTime(std::time_t time) noexcept -> void;

      private:
        static constexpr auto digitsCount                                     = 4;
        static constexpr std::array<std::uint8_t, digitsCount> bitsPerDisplay = {2, 4, 3, 4};

        std::array<BinaryDigitDisplay *, digitsCount> displays{};
    };
} // namespace gui
