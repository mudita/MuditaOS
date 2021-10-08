// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include <cstdint>

#include <Math.hpp>

#include "Common.hpp"
#include "Item.hpp"
#include "Style.hpp"

namespace gui
{
    class Arc : public Item
    {
      public:
        class ShapeParams
        {
          public:
            ShapeParams &setCenterPoint(Point _center) noexcept;
            ShapeParams &setRadius(Length _radius) noexcept;
            ShapeParams &setStartAngle(trigonometry::Degrees _angle) noexcept;
            ShapeParams &setSweepAngle(trigonometry::Degrees _angle) noexcept;
            ShapeParams &setBorderColor(Color _color) noexcept;
            ShapeParams &setPenWidth(std::uint8_t _width) noexcept;
            ShapeParams &setFocusPenWidth(std::uint8_t _width) noexcept;

          private:
            Point center;
            Length radius{0U};
            trigonometry::Degrees start{0U};
            trigonometry::Degrees sweep{0U};
            Color borderColor;
            std::uint8_t penWidth{1U};
            std::uint8_t focusPenWidth{style::window::default_border_focus_w};

            friend class Arc;
        };

        Arc(Item *parent, const ShapeParams &params);

        void setCenter(Point point) noexcept;
        void setSweepAngle(trigonometry::Degrees angle) noexcept;
        trigonometry::Degrees getSweepAngle() const noexcept;
        trigonometry::Degrees getStartAngle() const noexcept;

        void buildDrawListImplementation(std::list<Command> &commands) override;

      protected:
        Arc(Item *parent,
            Point _center,
            Length _radius,
            trigonometry::Degrees _start,
            trigonometry::Degrees _sweep,
            Color _color,
            std::uint8_t _penWidth,
            std::uint8_t _focusPenWidth);

        Point center;
        Length radius;
        trigonometry::Degrees start;
        trigonometry::Degrees sweep;
        Color color;
        std::uint8_t penWidth;
        std::uint8_t focusPenWidth;
    };
} // namespace gui
