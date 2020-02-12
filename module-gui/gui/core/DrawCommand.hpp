/*
 * DrawCommand.hpp
 *
 *  Created on: 25 kwi 2019
 *      Author: robert
 */

#ifndef GUI_CORE_DRAWCOMMAND_HPP_
#define GUI_CORE_DRAWCOMMAND_HPP_

#include <string>
#include <cstdint>

#include "utf8/UTF8.hpp"
#include "Color.hpp"
#include "../Common.hpp"


namespace gui {

/**
 * @brief IDs of the drawing commands
 */
enum class DrawCommandID {
	GUI_DRAW_UNDEFINED = 0,
	GUI_DRAW_CLEAR = 1,
	GUI_DRAW_LINE,
	GUI_DRAW_TRIANGLE,
	GUI_DRAW_RECT,
	GUI_DRAW_TEXT,
	GUI_DRAW_ARC,
	GUI_DRAW_PIE,
	GUI_DRAW_CIRCLE,
	GUI_DRAW_IMAGE,
	GUI_DRAW_COMMANDS, //blob of many
	GUI_RENDER_QUICK,
	GUI_RENDER_DEEP,
	GUI_RENDER_REFRESH //sent to applications so they can repaint current window
};

class DrawCommand {
public:

	DrawCommandID id;

	int16_t areaX;
	int16_t areaY;
	int16_t areaW;
	int16_t areaH;

	DrawCommand() : id{DrawCommandID::GUI_DRAW_UNDEFINED}, areaX{0}, areaY{0}, areaW{0}, areaH{0} {};
	virtual ~DrawCommand() {};
};

/**
 * @brief Draw command for rendering.
 */
class CommandRender : public DrawCommand
{
public:
	std::string applicationName;
	CommandRender() { id = DrawCommandID::GUI_RENDER_DEEP; };
};

/**
 * @brief Draw command for line.
 */
class CommandLine : public DrawCommand {
public:
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	Color color;
	uint8_t penWidth;

	CommandLine() : x1{0}, y1{0}, x2{0}, y2{0}, color( ColorFullBlack ),
		penWidth{1} {
		id = DrawCommandID::GUI_DRAW_LINE;
	}
};

/**
 * @brief Draw command for triangle.
 */
class CommandTriangle : DrawCommand {
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

	CommandTriangle() : x1{0}, y1{0}, x2{0}, y2{0}, x3{0}, y3{0},
		fillColor( ColorFullBlack ), borderColor( ColorFullBlack ), penWidth{1} {
		id = DrawCommandID::GUI_DRAW_TRIANGLE;
	}
};

/**
 * @brief Draw command for rectangle.
 */
class CommandRectangle : public DrawCommand {
public:
	int16_t x;
	int16_t y;
	uint16_t w;
	uint16_t h;
	uint16_t radius;
	//flags that defines whether paint given border
	RectangleEdgeFlags edges;
	//flags that defines which edge should be flat. This will disable roundness on both sides of the edge.
	RectangleFlatFlags flatEdges;
	//flags that defines whether paint given corner (only for rounded corners)
	RectangleCornerFlags corners;
    //flags indicating yaps for speech bubbles, it takes precendece over other properties
    RectangleYapFlags yaps;
    //defines which of the edges and corners are painted
    unsigned short yapSize = 0;
    bool filled;
	uint8_t penWidth;
	Color fillColor;
	Color borderColor;
    CommandRectangle() : x{0}, y{0}, w{0}, h{0},
			radius{0},
			edges{RectangleEdgeFlags::GUI_RECT_ALL_EDGES},
			flatEdges{RectangleFlatFlags::GUI_RECT_FLAT_NO_FLAT},
			corners{RectangleCornerFlags::GUI_RECT_ALL_CORNERS},
			yaps{RectangleYapFlags::GUI_RECT_YAP_NO_YAPS},
			yapSize{0},
			filled{false},
			penWidth{1},
			fillColor( ColorFullBlack ), borderColor( ColorFullBlack ) {
		id = DrawCommandID::GUI_DRAW_RECT;
	}
};

/**
 * @brief Draw command for circle.
 */
class CommandCircle : public DrawCommand
{
public:
	int16_t xc;
	int16_t yc;
	uint16_t r;
	uint8_t penWidth;
	bool filled;

	Color fillColor;
	Color borderColor;

	CommandCircle() : xc{0}, yc{0}, r{1}, penWidth{1}, filled{false},
		fillColor( ColorFullBlack ), borderColor( ColorFullBlack ) {
		id = DrawCommandID::GUI_DRAW_CIRCLE;
	}
};

/**
 * @brief Draw command for text line.
 */
class CommandText : public DrawCommand
{
public:
	//area where label wil lbe drawn
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
	//area occupied by text inside the label
	int16_t tx;
	int16_t ty;
	int16_t tw;
	int16_t th;
	uint16_t charsWidth;//number of visible pixels calculated
	UTF8 str;
	uint8_t fontID;

	Color color;

	CommandText() : x{0}, y{0}, w{0}, h{0}, tx{0}, ty{0}, tw{0}, th{0},
			charsWidth{0}, str{""}, fontID{ 0 }, color(ColorFullBlack) {
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

	//ID of the image
	uint16_t imageID;

	CommandImage() : x{0}, y{0}, w{0}, h{0}, imageID{0} {
		id = DrawCommandID::GUI_DRAW_IMAGE;
	}
};

} /* namespace gui */

#endif /* GUI_CORE_DRAWCOMMAND_HPP_ */
