// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui
{
    class BinaryDigitDisplay : public VBox
    {
      public:
        static constexpr auto maxBitsCount = 4;

        explicit BinaryDigitDisplay(Item *parent           = nullptr,
                                    std::uint32_t x        = 0,
                                    std::uint32_t y        = 0,
                                    std::uint8_t bitsCount = maxBitsCount);

        auto setValue(std::uint8_t value) noexcept -> void;
        [[nodiscard]] auto getValue() const noexcept -> std::uint8_t;

      private:
        std::uint8_t currentValue{};
        std::uint8_t bitsCount{};
        std::array<Rect *, maxBitsCount> dots{};

        auto createDisplay() noexcept -> void;
        auto updateDisplay() noexcept -> void;
        [[nodiscard]] auto getBitAt(std::uint8_t pos) const noexcept -> bool;
    };
} // namespace gui
