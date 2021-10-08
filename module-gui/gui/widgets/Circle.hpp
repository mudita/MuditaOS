// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include <cstdint>
#include "Arc.hpp"
#include "Common.hpp"
#include "Style.hpp"

namespace gui
{
    class Circle : public Arc
    {
      public:
        class ShapeParams
        {
          public:
            ShapeParams &setCenterPoint(Point _center) noexcept;
            ShapeParams &setRadius(Length _radius) noexcept;
            ShapeParams &setBorderColor(Color _color) noexcept;
            ShapeParams &setFocusBorderColor(Color _color) noexcept;
            ShapeParams &setPenWidth(std::uint8_t _width) noexcept;
            ShapeParams &setFocusPenWidth(std::uint8_t _width) noexcept;
            ShapeParams &setFillColor(Color _color) noexcept;

          private:
            Point center;
            Length radius{0U};
            Color borderColor{ColorFullBlack};
            Color focusBorderColor{ColorFullBlack};
            std::uint8_t penWidth{1U};
            std::uint8_t focusPenWidth{style::window::default_border_focus_w};
            bool isFilled{false};
            Color fillColor{ColorNoColor};

            friend class Circle;
        };

        Circle(Item *parent, const Circle::ShapeParams &params);

        void buildDrawListImplementation(std::list<Command> &commands) override;

      private:
        Circle(Item *parent,
               Point _center,
               Length _radius,
               Color _borderColor,
               Color _focusBorderColor,
               std::uint8_t _penWidth,
               std::uint8_t _focusPenWidth,
               bool _filled,
               Color _fillColor);

        bool isFilled;
        Color fillColor;
        Color focusBorderColor;
    };
} // namespace gui
