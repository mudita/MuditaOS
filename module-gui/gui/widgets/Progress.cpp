/*
 * Progress.cpp
 *
 *  Created on: 22 mar 2019
 *      Author: robert
 */

#include "Progress.hpp"

namespace gui {

Progress::Progress() : Rect() {
	setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
	createWidgets();
}

Progress::Progress( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h) :
	Rect( parent, x, y, w, h ),
	total{0},
	current{0} {
	setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
	createWidgets();
	updateDrawArea();
}

void Progress::createWidgets() {
	//thin line occpy 20% of the height of the parent
	thinRect = new gui::Rect( this, 0, 2*widgetArea.h/5, widgetArea.w, widgetArea.h/5 );

	//thick rect is rounded, filled and occupy total height of parent widget
	thickRect = new gui::Rect( this, 0, 0, 0, widgetArea.h );
	thickRect->setRadius(widgetArea.h/2-1);
	thickRect->setFilled(true);
	thickRect->setFillColor( Color{0, 0});
}

void Progress::updateProgress() {
	int width = widgetArea.w;
    float thickWidth = 0.0f;
    if( total )
        thickWidth = current*width/total;
    if( thickWidth < 2*thickRect->radius )
    	thickWidth = 2*thickRect->radius+1;
    thickRect->setSize( static_cast<int>(thickWidth), thickRect->widgetArea.h );
}

void Progress::setTotalProgress( int value ) {
	if( value < 0 )
		value = 0;

	total = value;

	if( current > total )
		current = total;
	updateProgress();
}

void Progress::setCurrentProgress( int value ) {
	if( value < 0 )
		value = 0;
	if( value > total )
		value = total;
	current = value;
	updateProgress();
}

void Progress::setCurrentPercent( int value ) {
	int newValue = static_cast<int>((float)total*(float)value/100.0);
	setCurrentProgress(newValue);
}

std::list<DrawCommand*> Progress::buildDrawList() {

	std::list<DrawCommand*> commands;

	if( visible == false ) {
		return commands;
	}

	std::list<DrawCommand*> childrenCommands = Item::buildDrawList();
	std::list<DrawCommand*> baseCommands = gui::Rect::buildDrawList();

	if( !baseCommands.empty() )
		commands.insert( commands.end(), baseCommands.begin(), baseCommands.end());

	if( !childrenCommands.empty() )
		commands.insert( commands.end(), childrenCommands.begin(), childrenCommands.end());

	return commands;
}

bool Progress::onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) {
	thinRect->setSize(newDim.w, newDim.h/5);
	thinRect->setPosition(0, 2*newDim.h/5);
	thickRect->setSize(newDim.w, newDim.h);
	updateProgress();
	return true;
}

} /* namespace gui */
