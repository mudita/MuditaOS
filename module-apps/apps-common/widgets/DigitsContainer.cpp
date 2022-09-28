// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DigitsContainer.hpp"

#include <Utils.hpp>

namespace gui
{

    std::size_t minStringLength(const std::uint32_t number)
    {
        /// Add leading zero to 1-digit numbers
        return (number < 100) ? 2 : 3;
    }

    void setDigits(std::string text, const DigitsContainer &container, DimensionsParams params)
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

    DigitsContainer::DigitsContainer(uint32_t digitsCount) : digits(digitsCount, nullptr)
    {}

    void DigitsContainer::setMinutesBox(std::uint32_t minutes, DimensionsParams params)
    {
        constexpr auto rangeGuard = 1000;
        minutes %= rangeGuard;

        setDigits(utils::addLeadingZeros(std::to_string(minutes), minStringLength(minutes)), *this, params);
    }

    void DigitsContainer::setSecondsBox(std::uint32_t seconds, DimensionsParams params)
    {
        constexpr auto rangeGuard = 100;
        seconds %= rangeGuard;

        setDigits(utils::addLeadingZeros(std::to_string(seconds), digits.size()), *this, params);
    }

} // namespace gui
