// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>

namespace gui
{
    struct DimensionsParams
    {
        std::uint32_t mainBoxHeight;
        std::uint32_t mainBoxWidth;
        std::uint32_t digitMaxWidth;
        std::uint32_t colonWidth;
        std::uint32_t minusWidth;
        std::uint32_t leftBoxWidth;
        std::uint32_t rightBoxWidth;
    };

    template <size_t N> struct DigitsContainer
    {
        std::array<Label *, N> digits;
        void setMinutesBox(std::uint32_t minutes, DimensionsParams params);
        void setSecondsBox(std::uint32_t seconds, DimensionsParams params);
    };
    template <size_t N> void setDigits(std::string text, const DigitsContainer<N> &container, DimensionsParams params)
    {
        std::for_each(std::crbegin(container.digits),
                      std::crend(container.digits),
                      [text = std::move(text), &params](Label *label) mutable {
                          if (text.empty()) {
                              label->setText("");
                              label->setSize(0, 0);
                          }
                          else {
                              label->setText(std::string{text.back()});
                              label->setSize(params.digitMaxWidth, params.mainBoxHeight);
                              text.pop_back();
                          }
                      });
    }

    template <size_t N> void DigitsContainer<N>::setMinutesBox(std::uint32_t minutes, DimensionsParams params)
    {
        auto rangeGuard = 1000;
        minutes %= rangeGuard;

        setDigits(std::to_string(minutes), *this, params);
    }

    template <size_t N> void DigitsContainer<N>::setSecondsBox(std::uint32_t seconds, DimensionsParams params)
    {
        auto rangeGuard = 100;
        seconds %= rangeGuard;

        bool shouldBeDisplayedAsOneDigit = seconds < 10;
        std::string text = shouldBeDisplayedAsOneDigit ? "0" + std::to_string(seconds) : std::to_string(seconds);

        setDigits(std::move(text), *this, params);
    }
} // namespace gui
