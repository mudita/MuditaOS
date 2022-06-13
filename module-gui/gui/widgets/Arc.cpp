// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "Arc.hpp"
#include "DrawCommand.hpp"

namespace gui
{
    Arc::ShapeParams &Arc::ShapeParams::setCenterPoint(Point _center) noexcept
    {
        center = _center;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setRadius(Length _radius) noexcept
    {
        radius = _radius;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setStartAngle(trigonometry::Degrees _angle) noexcept
    {
        start = _angle;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setSweepAngle(trigonometry::Degrees _angle) noexcept
    {
        sweep = _angle;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setBorderColor(Color _color) noexcept
    {
        borderColor = _color;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setPenWidth(std::uint8_t _width) noexcept
    {
        penWidth = _width;
        return *this;
    }

    Arc::ShapeParams &Arc::ShapeParams::setFocusPenWidth(std::uint8_t _width) noexcept
    {
        focusPenWidth = _width;
        return *this;
    }

    Arc::Arc(Item *parent, const ShapeParams &params)
        : Arc{parent,
              params.center,
              params.radius,
              params.start,
              params.sweep,
              params.borderColor,
              params.penWidth,
              params.focusPenWidth}
    {}

    Arc::Arc(Item *parent,
             Point _center,
             Length _radius,
             trigonometry::Degrees _start,
             trigonometry::Degrees _sweep,
             Color _color,
             std::uint8_t _penWidth,
             std::uint8_t _focusPenWidth)
        : Item{}, center{_center}, radius{_radius}, start{_start}, sweep{_sweep}, color{_color}, penWidth{_penWidth},
          focusPenWidth{_focusPenWidth}
    {
        const auto x        = center.x - radius;
        const auto y        = center.y - radius;
        const auto diameter = 2 * radius;
        setArea(BoundingBox(x, y, diameter, diameter));
        setMinimumWidth(diameter);
        setMinimumHeight(diameter);

        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
    }

    void Arc::setCenter(Point point) noexcept
    {
        center = point;
    }

    void Arc::setStartAngle(trigonometry::Degrees angle) noexcept
    {
        start = angle;
    }

    void Arc::setSweepAngle(trigonometry::Degrees angle) noexcept
    {
        sweep = angle;
    }

    trigonometry::Degrees Arc::getSweepAngle() const noexcept
    {
        return sweep;
    }

    trigonometry::Degrees Arc::getStartAngle() const noexcept
    {
        return start;
    }

    void Arc::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto arc   = std::make_unique<DrawArc>(center, radius, start, sweep, focus ? focusPenWidth : penWidth, color);
        arc->areaX = widgetArea.x;
        arc->areaY = widgetArea.y;
        arc->areaW = widgetArea.w;
        arc->areaH = widgetArea.h;

        commands.emplace_back(std::move(arc));
    }
} // namespace gui
