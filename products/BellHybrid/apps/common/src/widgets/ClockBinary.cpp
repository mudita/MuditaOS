// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <widgets/ClockBinary.hpp>
#include <widgets/BinaryDigitDisplay.hpp>

namespace
{
    constexpr auto displaySideMargin{40};
}

namespace gui
{
    ClockBinary::ClockBinary(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h)
        : HBox{parent, x, y, w, h}
    {
        setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Center});
        setEdges(RectangleEdge::None);

        for (auto i = 0; i < digitsCount; ++i) {
            displays[i] = new BinaryDigitDisplay(this, 0, 0, bitsPerDisplay[i]);

            Margins margins{displaySideMargin, 0, displaySideMargin, 0};
            if (i == 0) {
                margins = {0, 0, displaySideMargin, 0};
            }
            else if (i == digitsCount - 1) {
                margins = {displaySideMargin, 0, 0, 0};
            }
            displays[i]->setMargins(margins);
        }
    }

    auto ClockBinary::setHour(int value) noexcept -> void
    {
        displays[0]->setValue(value / 10);
        displays[1]->setValue(value % 10);
    }

    auto ClockBinary::setMinute(int value) noexcept -> void
    {
        displays[2]->setValue(value / 10);
        displays[3]->setValue(value % 10);
    }

    auto ClockBinary::setTime(std::time_t time) noexcept -> void
    {
        const auto t = std::localtime(&time);
        setHour(t->tm_hour);
        setMinute(t->tm_min);
    }
} // namespace gui
