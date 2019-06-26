/*
 * Rect.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */

#include "../core/BoundingBox.hpp"
#include "../core/DrawCommand.hpp"

#include "Rect.hpp"

namespace gui {

Rect::Rect() :
	borderColor(Color(0,0)),
	fillColor(Color(15,15)),
	penWidth{1},
	penFocusWidth{1},
	filled{false},
	edges{RectangleEdgeFlags::GUI_RECT_ALL_EDGES},
	flatEdges{ RectangleFlatFlags::GUI_RECT_FLAT_NO_FLAT},
	corners{RectangleCornerFlags::GUI_RECT_ALL_CORNERS} {
}

Rect::Rect( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	borderColor(Color(0,0)),
	fillColor(Color(15,15)),
	penWidth{1},
	penFocusWidth{1},
	filled{false},
	edges{RectangleEdgeFlags::GUI_RECT_ALL_EDGES},
	flatEdges{ RectangleFlatFlags::GUI_RECT_FLAT_NO_FLAT},
	corners{RectangleCornerFlags::GUI_RECT_ALL_CORNERS} {

	widgetArea.x = 0;
	widgetArea.y = 0;
	widgetArea.w = 0;
	widgetArea.h = 0;
	this->parent = parent;
	parent->addWidget(this);

	setPosition(x, y);
	setSize(w, h);
}

Rect::~Rect() {
}

void Rect::setFillColor( const Color& color ) {
	fillColor = color;
}

void Rect::setBorderColor( const Color& color ) {
	borderColor = color;
}
void Rect::setPenWidth( uint8_t width ) {
	penWidth = width;
}
void Rect::setPenFocusWidth( uint8_t width ) {
	penFocusWidth = width;
}

void Rect::setEdges( RectangleEdgeFlags edges ) {
	this->edges = edges;
}
void Rect::setCorners( RectangleCornerFlags corners ) {
	this->corners = corners;
}

void Rect::setFlat( RectangleFlatFlags flats)
{
	flatEdges = flats;
}

void Rect::setFilled( bool val ) {
	filled = val;
}

std::list<DrawCommand*> Rect::buildDrawList() {

	std::list<DrawCommand*> commands;

	//check if widget is visible
	if( visible == false ) {
		return commands;
	}

	//get children draw commands
	std::list<DrawCommand*> childrenCommands = Item::buildDrawList();

	//set local draw commands
	CommandRectangle* rect = new CommandRectangle();

	rect->x = drawArea.x;
	rect->y = drawArea.y;
	rect->w = drawArea.w;
	rect->h = drawArea.h;
	rect->areaX = widgetArea.x;
	rect->areaY = widgetArea.y;
	rect->areaW = widgetArea.w;
	rect->areaH = widgetArea.h;
	rect->corners = corners;
	rect->flatEdges = this->flatEdges;
	rect->edges =  edges;

	rect->radius = radius;
	if( focus )
		rect->penWidth = penFocusWidth;
	else
		rect->penWidth = penWidth;

	rect->filled = filled;
	rect->borderColor = borderColor;
	rect->fillColor = fillColor;

	commands.push_back( rect );

	if( !childrenCommands.empty() )
		commands.insert( commands.end(), childrenCommands.begin(), childrenCommands.end());

	return commands;
}

} /* namespace gui */
