/*
 * Item.cpp
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#include <list>
#include <algorithm>
#include <cstring>
#include "Navigation.hpp"
#include "Item.hpp"
#include "BoundingBox.hpp"

namespace gui {

Item::Item() :
	focus {false},
	type {ItemType::ITEM},
	parent{nullptr},
	radius{0},
	enabled{true},
	visible{true},
	verticalPolicy{ LayoutVerticalPolicy::LAYOUT_POLICY_VERTICAL_EXPAND },
	horizontalPolicy { LayoutHorizontalPolicy::LAYOUT_POLICY_HORIZONTAL_EXPAND },
	minHeight { 0 },
	minWidth { 0 },
	maxHeight { 0xFFFF },
	maxWidth { 0xFFFF },
	navigationDirections{ nullptr } {

	focusChangedCallback = [](Item&){ return false;};
	activatedCallback = [](Item&){ return false;};
	inputCallback = [](Item&, InputEvent& inputEvent){ return false;};
	dimensionChangedCallback = [](Item&,  void* data){ return false;};
	contentCallback = [](Item&){ return false;};
}

Item::~Item() {
	for( auto& widget : children )
		delete widget;

	if( navigationDirections )
		delete navigationDirections;
}

bool Item::addWidget( Item* item ) {
    if( item == nullptr ) return false;
	if( item->parent ) {
		item->parent->removeWidget(item);
	}

	item->parent = this;
	children.push_back(item);

	item->updateDrawArea();
	return true;
}

bool Item::removeWidget( Item* item ) {
	auto fi = std::find( children.begin(), children.end(), item );
	if( fi != children.end() ) {
		children.erase(fi);
		return true;
	}
	return false;
}

void Item::setVisible( bool value ) {
	visible = value;
}

std::list<DrawCommand*> Item::buildDrawList() {

	std::list<DrawCommand*> commands;

	for( auto& widget : children ) {
		std::list<DrawCommand*> widgetCommands = widget->buildDrawList();
		if( !widgetCommands.empty() )
			commands.insert( commands.end(), widgetCommands.begin(), widgetCommands.end() );
	}

	return commands;
}

void Item::setPosition( const short& x, const short& y ) {

	BoundingBox oldArea = widgetArea;
	widgetArea.x = x;
	widgetArea.y = y;
	updateDrawArea();

	onDimensionChanged(oldArea, widgetArea);
}

void Item::setSize( const short& w, const short& h ) {

	BoundingBox oldArea = widgetArea;
	widgetArea.w = w;
	if( widgetArea.w < 0 )
		widgetArea.w = 0;
	widgetArea.h = h;
	if( widgetArea.h < 0 )
		widgetArea.h = 0;
	updateDrawArea();

	onDimensionChanged(oldArea, widgetArea);
}

void Item::setRadius( int value ) {
	if( value < 0 )
		value = 0;
	radius = value;
}

void Item::updateDrawArea() {
	Item* parentItem = parent;

	BoundingBox result = widgetArea;

	//iterate up to top widget in hierarchy
	while( parentItem != nullptr ) {

		result.x += parentItem->widgetArea.x;
		result.y += parentItem->widgetArea.y;
		BoundingBox newResult;
		if( BoundingBox::intersect(parentItem->widgetArea, result, newResult) == false ) {
			result.clear();
			break;
		}
		result = newResult;

		parentItem = parentItem->parent;
	}

	drawArea = result;

	for( gui::Item* it : children )
		it->updateDrawArea();
}

Item* Item::getNavigationItem( NavigationDirection direction ) {
	if( navigationDirections != nullptr ) {
		return navigationDirections->getDirectionItem(direction);
	}
	return nullptr;
}

void Item::setNavigationItem( gui::NavigationDirection direction, Item* item ) {
	if( navigationDirections == nullptr )
		navigationDirections = new Navigation();
	navigationDirections->setDirectionItem(direction, item);
}

uint32_t Item::getMinHeight() {
	return minHeight;
}

uint32_t Item::getMinWidth() {
	return minWidth;
}

uint32_t Item::getMaxHeight() {
	return maxHeight;
}

uint32_t Item::getMaxWidth() {
	return maxWidth;
}

void Item::setMaxSize( const uint16_t& w, const uint16_t& h) {
	maxWidth = w;
	maxHeight = h;
}
void Item::setMinSize( const uint16_t& w, const uint16_t& h) {
	minWidth = w;
	minHeight = h;
}

} /* namespace gui */
