// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <cstdint>
#include <Math.hpp>
#include <utf8/UTF8.hpp>
#include <gui/Common.hpp>

#include "Color.hpp"
#include "Context.hpp"
#include <FontGlyph.hpp>

// image
#include "PixMap.hpp"
#include "VecMap.hpp"

namespace gui
{
    /**
     * @brief Draw command interface.
     */
    class DrawCommand
    {
      public:
        int16_t areaX{0};
        int16_t areaY{0};
        Length areaW{0};
        Length areaH{0};

        virtual ~DrawCommand() = default;

        virtual void draw(Context *ctx) const = 0;
    };

    class Clear : public DrawCommand
    {
        void draw(Context *ctx) const override;
    };

    /**
     * @brief Draw command for line.
     */
    class DrawLine : public DrawCommand
    {
      public:
        Point start{0, 0};
        Point end{0, 0};
        Color color{ColorFullBlack};
        uint8_t penWidth{1};

      public:
        void draw(Context *ctx) const override;
    };

    /**
     * @brief Draw command for rectangle.
     */
    class DrawRectangle : public DrawCommand
    {
      public:
        Point origin{0, 0};
        Length width{0};
        Length height{0};
        Length radius{0};

        // flags that defines whether paint given border
        RectangleEdge edges{RectangleEdge::All};
        // flags that defines which edge should be flat. This will disable roundness on both sides of the edge.
        RectangleFlatEdge flatEdges{RectangleFlatEdge::None};
        // flags that defines whether paint given corner (only for rounded corners)
        RectangleRoundedCorner corners{RectangleRoundedCorner::All};
        // flags indicating yaps for speech bubbles, it takes precendece over other properties
        RectangleYap yaps{RectangleYap::None};
        // defines which of the edges and corners are painted
        unsigned short yapSize{0};

        bool filled{false};
        uint8_t penWidth{1};
        Color fillColor{ColorFullBlack};
        Color borderColor{ColorFullBlack};

      public:
        void draw(Context *ctx) const override;
    };

    /**
     * @brief Draw command for arc.
     */
    class DrawArc : public DrawCommand
    {
      public:
        const trigonometry::Degrees start;
        const trigonometry::Degrees sweep;
        const Length width;
        const Color borderColor;
        const Point center;
        const Length radius;

        DrawArc(Point _center,
                Length _radius,
                trigonometry::Degrees _start,
                trigonometry::Degrees _sweep,
                Length _width,
                Color _color)
            : start{_start}, sweep{_sweep}, width{_width}, borderColor{_color}, center{_center}, radius{_radius}
        {}

        void draw(Context *ctx) const override;
    };

    /**
     * @brief Draw command for circle.
     */
    class DrawCircle : public DrawArc
    {
      public:
        const bool filled;
        const Color fillColor;

        DrawCircle(Point _center,
                   Length _radius,
                   Length _borderWidth,
                   Color _borderColor,
                   bool _filled     = false,
                   Color _fillColor = {})
            : DrawArc{_center, _radius, 0, trigonometry::FullAngle, _borderWidth, _borderColor}, filled{_filled},
              fillColor{_fillColor}
        {}

        void draw(Context *ctx) const override;
    };

    /**
     * @brief Draw command for text line.
     */
    class DrawText : public DrawCommand
    {
      public:
        // area where label wil be drawn
        Point origin{0, 0};
        Length width{0};
        Length height{0};
        // area occupied by text inside the label
        Point textOrigin{0, 0};
        Length textHeight{0};

        UTF8 str{};
        uint8_t fontID{0};
        Color color{ColorFullBlack};

        void draw(Context *ctx) const override;

      private:
        void drawChar(Context *ctx, const Point glyphOrigin, FontGlyph *glyph) const;
    };

    /**
     * @brief Draw command for image.
     */
    class DrawImage : public DrawCommand
    {
      public:
        Point origin{0, 0};

        // ID of the image
        uint16_t imageID{0};

        void draw(Context *ctx) const override;

      private:
        void drawPixMap(Context *ctx, PixMap *pixMap) const;
        void drawVecMap(Context *ctx, VecMap *vecMap) const;
        inline void checkImageSize(Context *ctx, ImageMap *image) const;
    };

} /* namespace gui */
