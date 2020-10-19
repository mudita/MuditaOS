#include <module-utils/log/log.hpp>
#include "Circle.hpp"
#include "DrawCommand.hpp"

namespace gui
{
    Circle::ShapeParams &Circle::ShapeParams::setCenterPoint(Point _center) noexcept
    {
        center = _center;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setRadius(Length _radius) noexcept
    {
        radius = _radius;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setBorderColor(Color _color) noexcept
    {
        borderColor = _color;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setFocusBorderColor(Color _color) noexcept
    {
        focusBorderColor = _color;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setPenWidth(std::uint8_t _width) noexcept
    {
        penWidth = _width;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setFocusPenWidth(std::uint8_t _width) noexcept
    {
        focusPenWidth = _width;
        return *this;
    }

    Circle::ShapeParams &Circle::ShapeParams::setFillColor(Color _color) noexcept
    {
        isFilled  = true;
        fillColor = _color;
        return *this;
    }

    Circle::Circle(Item *parent, const Circle::ShapeParams &params)
        : Circle{parent,
                 params.center,
                 params.radius,
                 params.borderColor,
                 params.focusBorderColor,
                 params.penWidth,
                 params.focusPenWidth,
                 params.isFilled,
                 params.fillColor}
    {}

    Circle::Circle(Item *parent,
                   Point _center,
                   Length _radius,
                   Color _borderColor,
                   Color _focusBorderColor,
                   std::uint8_t _penWidth,
                   std::uint8_t _focusPenWidth,
                   bool _filled,
                   Color _fillColor)
        : Arc{parent, _center, _radius, 0, trigonometry::FullAngle, _borderColor, _penWidth, _focusPenWidth},
          isFilled{_filled}, fillColor{_fillColor}, focusBorderColor{_focusBorderColor}
    {}

    std::list<DrawCommand *> Circle::buildDrawList()
    {
        if (!visible) {
            return {};
        }

        auto circle = new CommandCircle(
            center, radius, focus ? focusPenWidth : penWidth, focus ? focusBorderColor : color, isFilled, fillColor);
        circle->areaX = widgetArea.x;
        circle->areaY = widgetArea.y;
        circle->areaW = widgetArea.w;
        circle->areaH = widgetArea.h;

        std::list<DrawCommand *> commands;
        commands.push_back(circle);

        auto childrenCommands = Item::buildChildrenDrawList();
        commands.splice(commands.end(), childrenCommands);
        return commands;
    }
} // namespace gui
