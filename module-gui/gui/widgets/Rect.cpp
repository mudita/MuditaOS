// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Rect.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */

#include "../core/BoundingBox.hpp"
#include "../core/DrawCommand.hpp"

#include "Rect.hpp"
#include "Style.hpp"
#include <log/log.hpp>

namespace gui
{

    Rect::Rect(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
    {
        setArea({int16_t(x), int16_t(y), uint16_t(w), uint16_t(h)});
        setMinimumHeight(h);
        setMinimumWidth(w);

        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
    }

    Rect::Rect() : Rect(nullptr, 0, 0, 0, 0)
    {}

    void Rect::setFillColor(const Color &color)
    {
        fillColor = color;
        setFilled(true);
    }

    void Rect::setBorderColor(const Color &color)
    {
        borderColor = color;
    }
    void Rect::setPenWidth(uint8_t width)
    {
        penWidth = width;
    }
    void Rect::setPenFocusWidth(uint8_t width)
    {
        penFocusWidth = width;
    }

    void Rect::setEdges(RectangleEdge edges)
    {
        this->edges = edges;
    }
    void Rect::setCorners(RectangleRoundedCorner corners)
    {
        this->corners = corners;
    }

    void Rect::setFlat(RectangleFlatEdge flats)
    {
        flatEdges = flats;
    }

    void Rect::setFilled(bool val)
    {
        filled = val;
    }

    void Rect::setYaps(RectangleYap yaps)
    {
        this->yaps = yaps;
        if (yaps & (RectangleYap::BottomLeft | RectangleYap::TopLeft)) {
            padding.left = yapSize;
        }
        else {
            padding.left = 0;
        }
        if (yaps & (RectangleYap::BottomRight | RectangleYap::TopRight)) {
            padding.right = yapSize;
        }
        else {
            padding.right = 0;
        }
    }

    void Rect::setYapSize(unsigned short value)
    {
        yapSize = value;
    }

    void Rect::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto rect = std::make_unique<DrawRectangle>();

        rect->origin    = {drawArea.x, drawArea.y};
        rect->width     = drawArea.w;
        rect->height    = drawArea.h;
        rect->areaX     = widgetArea.x;
        rect->areaY     = widgetArea.y;
        rect->areaW     = widgetArea.w;
        rect->areaH     = widgetArea.h;
        rect->corners   = corners;
        rect->flatEdges = this->flatEdges;
        rect->edges     = edges;
        rect->yaps      = yaps;
        rect->yapSize   = yapSize;
        rect->radius    = radius;
        if (focus) {
            rect->penWidth = penFocusWidth;
        }
        else {
            rect->penWidth = penWidth;
        }

        rect->filled      = filled;
        rect->borderColor = borderColor;
        rect->fillColor   = fillColor;

        commands.emplace_back(std::move(rect));
    }

    void Rect::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }

} /* namespace gui */
