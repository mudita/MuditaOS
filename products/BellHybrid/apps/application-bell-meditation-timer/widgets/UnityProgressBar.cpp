// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <Math.hpp>

#include "DrawCommand.hpp"
#include "UnityProgressBar.hpp"

namespace gui
{
    UnityProgressBar::UnityProgressBar(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::uint32_t faciaWidth, std::uint32_t gapWidth)
        : Rect(parent, x, y, w, h), faciaWidth{faciaWidth}, gapWidth{gapWidth}, displayWidth{w}, displayHeight{h}
    {
        setFilled(false);
        updateDrawArea();
    }

    void UnityProgressBar::setMaximum(unsigned int value) noexcept
    {
        maxValue = value;
        if (currentValue > maxValue) {
            currentValue = maxValue;
        }
    }

    bool UnityProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
        return currentValue == value;
    }

    void UnityProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }

    int UnityProgressBar::getMaximum() const noexcept
    {
        return maxValue;
    }

    void UnityProgressBar::buildDrawListImplementation(std::list<Command> &commands)
    {
        uint32_t items = (displayWidth + gapWidth) / (faciaWidth + gapWidth);
        uint32_t count = currentValue * items / maxValue;

        uint32_t fx;
        uint32_t fy;

        switch (alignment.vertical) {
        case Alignment::Vertical::Top:
            fy = drawArea.y;
            break;
        case Alignment::Vertical::Bottom:
            fy = drawArea.y + drawArea.h - displayHeight;
            break;
        case Alignment::Vertical::Center:
        default:
            fy = drawArea.y + (drawArea.h - displayHeight) / 2;
            break;
        }

        switch (alignment.horizontal) {
        case Alignment::Horizontal::Left:
            fx = drawArea.x;
            break;
        case Alignment::Horizontal::Right:
            fx = drawArea.x + drawArea.w - (faciaWidth * items + gapWidth * (items - 1));
            break;
        case Alignment::Horizontal::Center:
        default:
            fx = drawArea.x + (drawArea.w - (faciaWidth * items + gapWidth * (items - 1))) / 2;
            break;
        }

        for (uint32_t i = 0; i < items; i++) {
            gui::Position x = static_cast<gui::Position>(fx + (faciaWidth + gapWidth) * i);
            gui::Position y = static_cast<gui::Position>(fy);
            auto rect       = std::make_unique<DrawRectangle>();
            rect->origin    = {x, y};
            rect->width     = faciaWidth;
            rect->height    = displayHeight;
            rect->areaX     = x;
            rect->areaY     = widgetArea.y;
            rect->areaW     = faciaWidth;
            rect->areaH     = widgetArea.h;
            rect->filled    = true;
            if (i < count) {
                rect->fillColor   = ColorFullBlack;
                rect->borderColor = ColorFullBlack;
            }
            else {
                rect->fillColor   = ColorGrey;
                rect->borderColor = ColorGrey;
            }
            commands.emplace_back(std::move(rect));
        }
    }

    bool UnityProgressBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        return true;
    }

    void UnityProgressBar::setFaciaSize(std::uint32_t fw, std::uint32_t gw)
    {
        faciaWidth = fw;
        gapWidth   = gw;
    }

    void UnityProgressBar::setDisplaySize(std::uint32_t width, std::uint32_t height)
    {
        displayWidth  = width;
        displayHeight = height;
    }
} /* namespace gui */
