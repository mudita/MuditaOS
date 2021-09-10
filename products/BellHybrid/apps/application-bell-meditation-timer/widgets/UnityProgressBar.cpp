// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <Math.hpp>

#include "DrawCommand.hpp"
#include "UnityProgressBar.hpp"

namespace gui
{
    UnityProgressBar::UnityProgressBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : ProgressBar(parent, x, y, w, h)
    {
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

    void UnityProgressBar::buildDrawListImplementation(std::list<Command> &commands)
    {
        uint32_t items = (widgetArea.w + gw) / (rw + gw);
        uint32_t count = currentValue * items / maxValue;
        uint32_t fx    = widgetArea.x + (widgetArea.w - (rw * items + gw * (items - 1))) / 2;

        for (uint32_t i = 0; i < items; i++) {
            gui::Position x = static_cast<gui::Position>(fx + (rw + gw) * i);
            auto rect       = std::make_unique<DrawRectangle>();
            rect->origin    = {x, drawArea.y};
            rect->width     = rw;
            rect->height    = drawArea.h;
            rect->areaX     = x;
            rect->areaY     = widgetArea.y;
            rect->areaW     = rw;
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
} /* namespace gui */
