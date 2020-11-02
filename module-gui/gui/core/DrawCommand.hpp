// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <cstdint>
#include <module-utils/math/Math.hpp>
#include <utf8/UTF8.hpp>
#include <gui/Common.hpp>

#include "Color.hpp"

namespace gui
{

    enum class DrawCommandID
    {
        GUI_DRAW_UNDEFINED = 0,
        GUI_DRAW_CLEAR     = 1,
        GUI_DRAW_LINE,
        GUI_DRAW_TRIANGLE,
        GUI_DRAW_RECT,
        GUI_DRAW_TEXT,
        GUI_DRAW_ARC,
        GUI_DRAW_PIE,
        GUI_DRAW_CIRCLE,
        GUI_DRAW_IMAGE,
        GUI_DRAW_COMMANDS, // blob of many
        GUI_RENDER_QUICK,
        GUI_RENDER_DEEP,
        GUI_RENDER_REFRESH // sent to applications so they can repaint current window
    };

    class DrawCommand
    {
      public:
        DrawCommandID id;

        int16_t areaX;
        int16_t areaY;
        int16_t areaW;
        int16_t areaH;

        DrawCommand() : id{DrawCommandID::GUI_DRAW_UNDEFINED}, areaX{0}, areaY{0}, areaW{0}, areaH{0} {};
        virtual ~DrawCommand(){};
    };

    class CommandRender : public DrawCommand
    {
      public:
        std::string applicationName;
        CommandRender()
        {
            id = DrawCommandID::GUI_RENDER_DEEP;
        };
    };

    class CommandLine : public DrawCommand
    {
      public:
        int16_t x1;
        int16_t y1;
        int16_t x2;
        int16_t y2;
        Color color;
        uint8_t penWidth;

        CommandLine() : x1{0}, y1{0}, x2{0}, y2{0}, color(ColorFullBlack), penWidth{1}
        {
            id = DrawCommandID::GUI_DRAW_LINE;
        }
    };

    /**
     * @brief Draw command for triangle.
     */
    class CommandTriangle : DrawCommand
    {
      public:
        int16_t x1;
        int16_t y1;
        int16_t x2;
        int16_t y2;
        int16_t x3;
        int16_t y3;
        Color fillColor;
        gui::Color borderColor;
        uint8_t penWidth;

        CommandTriangle()
            : x1{0}, y1{0}, x2{0}, y2{0}, x3{0}, y3{0}, fillColor(ColorFullBlack),
              borderColor(ColorFullBlack), penWidth{1}
        {
            id = DrawCommandID::GUI_DRAW_TRIANGLE;
        }
    };

    /**
     * @brief Draw command for rectangle.
     */
    class CommandRectangle : public DrawCommand
    {
      public:
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        uint16_t radius;
        // flags that defines whether paint given border
        RectangleEdge edges;
        // flags that defines which edge should be flat. This will disable roundness on both sides of the edge.
        RectangleFlatEdge flatEdges;
        // flags that defines whether paint given corner (only for rounded corners)
        RectangleRoundedCorner corners;
        // flags indicating yaps for speech bubbles, it takes precendece over other properties
        RectangleYap yaps;
        // defines which of the edges and corners are painted
        unsigned short yapSize = 0;
        bool filled;
        uint8_t penWidth;
        Color fillColor;
        Color borderColor;
        CommandRectangle()
            : x{0}, y{0}, w{0}, h{0}, radius{0}, edges{RectangleEdge::All}, flatEdges{RectangleFlatEdge::None},
              corners{RectangleRoundedCorner::All}, yaps{RectangleYap::None}, yapSize{0}, filled{false}, penWidth{1},
              fillColor(ColorFullBlack), borderColor(ColorFullBlack)
        {
            id = DrawCommandID::GUI_DRAW_RECT;
        }
    };

    class CommandArc : public DrawCommand
    {
      public:
        CommandArc(Point _center,
                   Length _radius,
                   trigonometry::Degrees _start,
                   trigonometry::Degrees _sweep,
                   uint8_t _width,
                   Color _color)
            : center{_center}, radius{_radius}, start{_start}, sweep{_sweep}, width{_width}, borderColor{_color}
        {
            id = DrawCommandID::GUI_DRAW_ARC;
        }

        const Point center;
        const Length radius;
        const trigonometry::Degrees start;
        const trigonometry::Degrees sweep;
        const uint8_t width;
        const Color borderColor;
    };

    /**
     * @brief Draw command for circle.
     */
    class CommandCircle : public CommandArc
    {
      public:
        CommandCircle(Point _center,
                      Length _radius,
                      uint8_t _borderWidth,
                      Color _borderColor,
                      bool _filled     = false,
                      Color _fillColor = {})
            : CommandArc{_center, _radius, 0, trigonometry::FullAngle, _borderWidth, _borderColor}, filled{_filled},
              fillColor{_fillColor}
        {
            id = DrawCommandID::GUI_DRAW_CIRCLE;
        }

        const bool filled;
        const Color fillColor;
    };

    /**
     * @brief Draw command for text line.
     */
    class CommandText : public DrawCommand
    {
      public:
        // area where label wil lbe drawn
        int16_t x;
        int16_t y;
        int16_t w;
        int16_t h;
        // area occupied by text inside the label
        int16_t tx;
        int16_t ty;
        int16_t tw;
        int16_t th;
        uint16_t charsWidth; // number of visible pixels calculated
        UTF8 str;
        uint8_t fontID;

        Color color;

        CommandText()
            : x{0}, y{0}, w{0}, h{0}, tx{0}, ty{0}, tw{0}, th{0}, charsWidth{0}, str{""}, fontID{0},
              color(ColorFullBlack)
        {
            id = DrawCommandID::GUI_DRAW_TEXT;
        }
    };

    /**
     * @brief Draw command for image.
     */
    class CommandImage : public DrawCommand
    {
      public:
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;

        // ID of the image
        uint16_t imageID;

        CommandImage() : x{0}, y{0}, w{0}, h{0}, imageID{0}
        {
            id = DrawCommandID::GUI_DRAW_IMAGE;
        }
    };

} /* namespace gui */
