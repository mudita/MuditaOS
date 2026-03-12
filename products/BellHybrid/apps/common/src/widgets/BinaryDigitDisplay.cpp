// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <widgets/BinaryDigitDisplay.hpp>

#include <cassert>

namespace
{
    namespace dot
    {
        constexpr auto radiusEnabled{12};
        constexpr auto radiusDisabled{6};

        constexpr auto edgeLengthEnabled{radiusEnabled * 2};
        constexpr auto edgeLengthDisabled{radiusDisabled * 2};

        constexpr auto marginSizeEnabled{26};
        constexpr auto marginSizeDisabled{marginSizeEnabled + (radiusEnabled - radiusDisabled)};
    } // namespace dot

    namespace display
    {
        constexpr auto maxWidth{dot::edgeLengthEnabled};
        constexpr auto maxHeight{gui::BinaryDigitDisplay::maxBitsCount *
                                 (2 * dot::marginSizeEnabled + dot::edgeLengthEnabled)};
    } // namespace display
} // namespace

namespace gui
{
    BinaryDigitDisplay::BinaryDigitDisplay(Item *parent, std::uint32_t x, std::uint32_t y, std::uint8_t bitsCount)
        : VBox{parent, x, y, display::maxWidth, display::maxHeight}, bitsCount{bitsCount}
    {
        assert(this->bitsCount <= maxBitsCount);

        createDisplay();
        updateDisplay();
    }

    auto BinaryDigitDisplay::setValue(std::uint8_t value) noexcept -> void
    {
        currentValue = value;
        updateDisplay();
    }

    auto BinaryDigitDisplay::getValue() const noexcept -> std::uint8_t
    {
        return currentValue;
    }

    auto BinaryDigitDisplay::createDisplay() noexcept -> void
    {
        setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Bottom});
        setEdges(RectangleEdge::None);

        for (auto &dot : dots) {
            dot = new Rect(this, 0, 0, 0, 0);
            dot->setAlignment({Alignment::Horizontal::Center, Alignment::Vertical::Center});
            dot->setFillColor(ColorFullBlack);
            dot->setVisible(false);
        }
        std::reverse(dots.begin(), dots.end()); // Make top-most dot most significant bit
    }

    auto BinaryDigitDisplay::updateDisplay() noexcept -> void
    {
        for (auto i = 0; i < bitsCount; ++i) {
            if (getBitAt(i)) {
                dots[i]->setSize(dot::edgeLengthEnabled, dot::edgeLengthEnabled);
                dots[i]->setMaximumSize(dot::edgeLengthEnabled, dot::edgeLengthEnabled);
                dots[i]->setRadius(dot::radiusEnabled); // Hack to make rectangles circles - Rect class is easier to
                                                        // work with than Circle
                dots[i]->setMargins({0, dot::marginSizeEnabled, 0, dot::marginSizeEnabled});
            }
            else {
                dots[i]->setSize(dot::edgeLengthDisabled, dot::edgeLengthDisabled);
                dots[i]->setMaximumSize(dot::edgeLengthDisabled, dot::edgeLengthDisabled);
                dots[i]->setRadius(dot::radiusDisabled);
                dots[i]->setMargins({0, dot::marginSizeDisabled, 0, dot::marginSizeDisabled});
            }
            dots[i]->setVisible(true);
        }
        resizeItems();
    }

    auto BinaryDigitDisplay::getBitAt(std::uint8_t pos) const noexcept -> bool
    {
        return ((currentValue >> pos) & 1) != 0;
    }
} // namespace gui
